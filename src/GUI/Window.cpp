#include <QSettings>
#include <QFile>
#include <QEvent>
#include <QCloseEvent>
#include <QLabel>
#include <QProgressBar>
#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

#include "ui_Window.h"

#include "GUI/DocumentWindow.h"
#include "GUI/Window.h"
#include "Application/Document.h"
#include "Scene/SceneNode.h"
#include "Scene/SceneNodeSerializer.h"


using namespace Djbozkosz::Application::GUI;


const QString Window::OPEN_FILE_DIALOG_PATH   = "OpenFileDialogPath";
const QString Window::SAVE_FILE_DIALOG_PATH   = "SaveFileDialogPath";

const QString Window::IMPORT_FILE_DIALOG_PATH = "ImportFileDialogPath";
const QString Window::EXPORT_FILE_DIALOG_PATH = "ExportFileDialogPath";


Window::Window(QSettings* settings, Scene::Definitions* definitions) :
	QMainWindow(),
	m_NewFileCounter(1),
	m_Settings(settings),
	m_Definitions(definitions),
	m_Ui(new Ui::Window()),
	m_Status(null),
	m_Progress(null)
{
	m_Ui->setupUi(this);

	auto statusBar = m_Ui->StatusBar;

	m_Status   = new QLabel("Go to File... and open or create new file.");
	m_Progress = new QProgressBar();
	m_Progress->setMinimum(0);
	m_Progress->setMaximum(1000);
	m_Progress->setTextVisible(false);
	m_Progress->setVisible(false);

	statusBar->addWidget(m_Status, 2);
	statusBar->addWidget(m_Progress, 1);

	auto tabs = m_Ui->Tabs;
	tabs->installEventFilter(this);
	connect(tabs, SIGNAL(currentChanged(int)),    this, SLOT(UpdateEditMenu(int)));
	connect(tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(CloseFile(int)));

	connect(m_Ui->Menu_New,    SIGNAL(triggered()), this, SLOT(NewFile()));
	connect(m_Ui->Menu_Open,   SIGNAL(triggered()), this, SLOT(OpenFile()));
	connect(m_Ui->Menu_Reload, SIGNAL(triggered()), this, SLOT(ReloadFile()));
	connect(m_Ui->Menu_Save,   SIGNAL(triggered()), this, SLOT(SaveFile()));
	connect(m_Ui->Menu_SaveAs, SIGNAL(triggered()), this, SLOT(SaveAsFile()));
	connect(m_Ui->Menu_Close,  SIGNAL(triggered()), this, SLOT(CloseFile()));
	connect(m_Ui->Menu_Exit,   SIGNAL(triggered()), this, SLOT(ExitApp()));

	connect(m_Ui->Menu_Export, SIGNAL(triggered()), this, SLOT(ExportNode()));
	connect(m_Ui->Menu_Import, SIGNAL(triggered()), this, SLOT(ImportNode()));

	connect(m_Ui->Menu_About,  SIGNAL(triggered()), this, SLOT(ShowAbout()));
}

Window::~Window()
{
	disconnect(m_Ui->Menu_New,    SIGNAL(triggered()), this, SLOT(NewFile()));
	disconnect(m_Ui->Menu_Open,   SIGNAL(triggered()), this, SLOT(OpenFile()));
	disconnect(m_Ui->Menu_Reload, SIGNAL(triggered()), this, SLOT(ReloadFile()));
	disconnect(m_Ui->Menu_Save,   SIGNAL(triggered()), this, SLOT(SaveFile()));
	disconnect(m_Ui->Menu_SaveAs, SIGNAL(triggered()), this, SLOT(SaveAsFile()));
	disconnect(m_Ui->Menu_Close,  SIGNAL(triggered()), this, SLOT(CloseFile()));
	disconnect(m_Ui->Menu_Exit,   SIGNAL(triggered()), this, SLOT(ExitApp()));

	disconnect(m_Ui->Menu_Export, SIGNAL(triggered()), this, SLOT(ExportNode()));
	disconnect(m_Ui->Menu_Import, SIGNAL(triggered()), this, SLOT(ImportNode()));

	disconnect(m_Ui->Menu_About,  SIGNAL(triggered()), this, SLOT(ShowAbout()));

	auto tabs = m_Ui->Tabs;
	tabs->removeEventFilter(this);
	disconnect(tabs, SIGNAL(currentChanged(int)),    this, SLOT(UpdateEditMenu(int)));
	disconnect(tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(CloseFile(int)));

	delete m_Ui;
}

void Window::AddDocument(Document* document)
{
	m_Status->setText("Loaded. Refreshing interface...");

	auto tab = new DocumentWindow(document, m_Definitions, m_Ui->Tabs);

	connect(tab,      SIGNAL(ProgressChanged(float)),        this, SLOT(UpdateProgress(float)));
	connect(tab,      SIGNAL(EditMenuUpdateRequested()),     this, SLOT(UpdateEditMenu()));
	connect(tab,      SIGNAL(EditMenuShowRequested(QPoint)), this, SLOT(ShowEditMenu(QPoint)));
	connect(document, SIGNAL(DirtyStateChanged(bool)),       this, SLOT(UpdateDirtyState(bool)));

	tab->SetupTree();

	auto tabs = m_Ui->Tabs;

	tabs->addTab(tab, GetFileName(document));
	tabs->setCurrentIndex(tabs->count() - 1);

	m_Ui->Menu_Save->setEnabled(true);
	m_Ui->Menu_SaveAs->setEnabled(true);
	m_Ui->Menu_Close->setEnabled(true);

	m_Status->setText("Done.");
}

void Window::RemoveDocumemt(Djbozkosz::Application::Document* document)
{
	auto tab    = default_(DocumentWindow*);
	auto tabIdx = -1;

	for (int idx = 0, count = m_Ui->Tabs->count(); idx < count; idx++)
	{
		auto tabToCheck = GetTab(idx);
		if (tabToCheck->GetDocument() == document)
		{
			tab    = tabToCheck;
			tabIdx = idx;
			break;
		}
	}

	Debug::Assert(tab != null) << "Tab to remove not found!";

	disconnect(tab,      SIGNAL(ProgressChanged(float)),        this, SLOT(UpdateProgress(float)));
	disconnect(tab,      SIGNAL(EditMenuUpdateRequested()),     this, SLOT(UpdateEditMenu()));
	disconnect(tab,      SIGNAL(EditMenuShowRequested(QPoint)), this, SLOT(ShowEditMenu(QPoint)));
	disconnect(document, SIGNAL(DirtyStateChanged(bool)),       this, SLOT(UpdateDirtyState(bool)));

	auto tabs = m_Ui->Tabs;
	tabs->removeTab(tabIdx);
	tab->deleteLater();

	if (tabs->count() == 0)
	{
		m_Ui->Menu_Save->setEnabled(false);
		m_Ui->Menu_SaveAs->setEnabled(false);
		m_Ui->Menu_Close->setEnabled(false);

		auto actions = m_Ui->Menu_Edit->actions();
		foreach (action, actions)
		{
			(*action)->setEnabled(false);
		}
	}
}

bool Window::eventFilter(QObject* object, QEvent* event)
{
	if (object == m_Ui->Tabs && event->type() == QEvent::MouseButtonDblClick)
	{
		OpenFile();
	}

	return QObject::eventFilter(object, event);
}

void Window::closeEvent(QCloseEvent* event)
{
	if (ExitApp() == true)
	{
		event->accept();
		QMainWindow::closeEvent(event);
	}
	else
	{
		event->ignore();
	}
}

void Window::NewFile()
{
	emit FileCreated(m_NewFileCounter++);
}

void Window::OpenFile()
{
	auto file = ShowFileDialog(false, false, "Open file", OPEN_FILE_DIALOG_PATH, m_Definitions->GetDialogFiles());
	if (file.isEmpty() == true)
		return;

	emit FileOpened(file);
}

void Window::ReloadFile()
{
	auto tab      = GetCurrentTab();
	auto document = tab->GetDocument();
	auto result   = SaveIfDialog(document);

	if (result == false)
		return;

	emit FileReloaded(document);
	tab->SetupTree();
	document->SetDirty(false);
}

void Window::SaveFile()
{
	SaveDocument(GetCurrentDocument(), true);
}

void Window::SaveAsFile()
{
	SaveDocument(GetCurrentDocument(), false);
}

bool Window::SaveIfDialog(Document* document)
{
	auto result = 1;

	if (document->IsDirty() == true)
	{
		result = QMessageBox::question(this, windowTitle(), QString("Save changed file \"%1\"?").arg(GetFileName(document, false)), "&Yes", "&No", "&Cancel", 0);
	}

	if (result == 2)
		return false;

	if (result == 0)
	{
		SaveDocument(document, true);
	}

	return true;
}

bool Window::CloseFile()
{
	return CloseFile(GetCurrentTabIdx());
}

bool Window::CloseFile(int idx)
{
	auto document = GetTab(idx)->GetDocument();
	auto result   = SaveIfDialog(document);

	if (result == false)
		return false;

	RemoveDocumemt(document);
	emit FileClosed(document);

	return true;
}

bool Window::ExitApp()
{
	auto tabs = m_Ui->Tabs;
	while (tabs->count() > 0)
	{
		if (CloseFile() == false)
			return false;
	}

	qApp->quit();
	return true;
}

void Window::ExportNode()
{
	auto node = GetCurrentTab()->GetSelectedSceneNode();
	auto name = SceneNodeUtility::GetNodeName(node, m_Definitions);

	auto file = ShowFileDialog(true, false, "Export node", EXPORT_FILE_DIALOG_PATH, m_Definitions->GetDialogExportFiles(), name);
	if (file.isEmpty() == true)
		return;

	QFile writer(file);
	auto result = writer.open(QIODevice::WriteOnly);
	if (result == false)
		return;

	SceneNodeSerializer::Serialize(writer, *node);
}

void Window::ImportNode()
{
	auto file = ShowFileDialog(false, false, "Import node", EXPORT_FILE_DIALOG_PATH, m_Definitions->GetDialogExportFiles());
	if (file.isEmpty() == true)
		return;

	QFile reader(file);
	auto result = reader.open(QIODevice::ReadOnly);
	if (result == false)
		return;

	auto tab     = GetCurrentTab();
	auto sibling = tab->GetSelectedNode();
	auto parent  = (sibling != null) ? as(sibling->parent(), NodeItem*)->Node : null;
	auto node    = SceneNodeSerializer::Deserialize(reader, parent, *m_Definitions);
	tab->AddNode(node);
}

void Window::ShowAbout()
{
	QMessageBox::about(this, windowTitle(), "todo");
}

void Window::UpdateEditMenu(int tabIdx)
{
	auto tab = (tabIdx == -1) ? GetCurrentTab() : GetTab(tabIdx);
	if (tab == null)
		return;

	auto selectedNode   = tab->GetSelectedNode();
	auto isNodeSelected = (selectedNode != null);
	auto actions        = m_Ui->Menu_Edit->actions();

	foreach (action, actions)
	{
		(*action)->setEnabled(isNodeSelected);
	}

	if (selectedNode != null && tab->GetDocument()->GetRoot() == selectedNode->Node)
	{
		m_Ui->Menu_Import->setEnabled(false);
	}
	else if (m_Ui->Tabs->count() > 0)
	{
		m_Ui->Menu_Import->setEnabled(true);
	}
}

void Window::ShowEditMenu(QPoint point)
{
	m_Ui->Menu_Edit->popup(point);
}

void Window::UpdateProgress(float value)
{
	auto wasVisible = m_Progress->isVisible();
	auto isVisible  = (value > 0.0f && value < 1.0f);

	if (wasVisible != isVisible)
	{
		m_Progress->setVisible(isVisible);
	}

	m_Progress->setValue(value * 1000.0f);
}

void Window::UpdateDirtyState(bool isDirty)
{
	unused(isDirty);

	auto document = GetCurrentDocument();
	auto file     = GetFileName(document);
	auto idx      = GetCurrentTabIdx();

	m_Ui->Tabs->setTabText(idx, file);
}

DocumentWindow* Window::GetTab(int idx) const
{
	return as(m_Ui->Tabs->widget(idx), DocumentWindow*);
}

DocumentWindow* Window::GetCurrentTab() const
{
	return GetTab(GetCurrentTabIdx());
}

int Window::GetCurrentTabIdx() const
{
	return m_Ui->Tabs->currentIndex();
}

Djbozkosz::Application::Document* Window::GetCurrentDocument() const
{
	return GetCurrentTab()->GetDocument();
}

void Window::SaveDocument(Document* document, bool replace)
{
	auto file = document->GetFile();

	if (replace == false || file.isEmpty() == true)
	{
		file = ShowFileDialog(true, false, "Save file", SAVE_FILE_DIALOG_PATH, m_Definitions->GetDialogFiles());
	}

	if (file.isEmpty() == true)
		return;

	m_Status->setText("Saving...");
	emit FileSaved(document, file);
	m_Status->setText("Saved.");
}

QString Window::GetFileName(Document* document, bool useDirtyState)
{
	auto fileName = document->GetFile();
	if (fileName.isEmpty() == true)
	{
		fileName = QString("new_file_%1").arg(document->GetIdx());
	}

	if (useDirtyState == false || document->IsDirty() == false)
		return fileName;

	return QString("%1*").arg(fileName);
}

QString Window::ShowFileDialog(bool isSave, bool isDirectory, const QString& title, const QString& settingsPath, const Definitions::DialogFiles& files, const QString& file)
{
	auto directory    = m_Settings->value(settingsPath).toString();
	auto fileToDialog = file;

	if ((fileToDialog.isEmpty() == true || fileToDialog.contains('.') == false) && files.size() > 0)
	{
		fileToDialog = QString("%1%2").arg(fileToDialog).arg(files[0].File);
	}

	directory = QString("%1/%2").arg(directory).arg(fileToDialog);

	QStringList filterList;
	foreach (f, files)
	{
		filterList.push_back(QString("%1 (%2)").arg(f->Name).arg(f->Filter));
	}
	auto filter = filterList.join(";;");

	auto fileFromDialog =
		isDirectory ? QFileDialog::getExistingDirectory(this, title, directory) :
		isSave      ? QFileDialog::getSaveFileName(this, title, directory, filter) :
					  QFileDialog::getOpenFileName(this, title, directory, filter);

	if (fileFromDialog.isEmpty() == false)
	{
		directory = QFileInfo(fileFromDialog).absoluteDir().path();
		m_Settings->setValue(settingsPath, directory);
	}

	return fileFromDialog;
}
