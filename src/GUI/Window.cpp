#include <QBuffer>
#include <QList>
#include <QVariant>
#include <QSettings>
#include <QFile>
#include <QEvent>
#include <QCloseEvent>
#include <QLabel>
#include <QProgressBar>
#include <QMainWindow>
#include <QMenu>
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
const QString Window::RECENT_FILE_PATH        = "RecentFilePath";

const QString Window::IMPORT_FILE_DIALOG_PATH = "ImportFileDialogPath";
const QString Window::EXPORT_FILE_DIALOG_PATH = "ExportFileDialogPath";

const QString Window::ABOUT_TEXT =
		QString("After short development<br>created by: djbozkosz<br><br>"
		"Version: %1<br>"
		"Copyright (c): 2005 - 2020<br><br>"
		"Found a bug or have some ideas how to improve this app?<br>Don't hesitate to contact me at <a href=\"mailto:t_ruzicka@email.cz\">t_ruzicka@email.cz</a>.<br><br>"
		"Web: <a href=\"http://www.djbozkosz.wz.cz\">www.djbozkosz.wz.cz</a><br>"
		"Facebook: <a href=\"https://www.facebook.com/tomas.ruzicka.73\">facebook.com/tomas.ruzicka.73</a><br>"
		"Twitter: <a href=\"https://twitter.com/djbozkosz\">twitter.com/djbozkosz</a><br>"
		"GitHub: <a href=\"https://www.github.com/djbozkosz\">github.com/djbozkosz</a><br>"
		"LinkedIn: <a href=\"https://www.linkedin.com/in/tom%C3%A1%C5%A1-r%C5%AF%C5%BEi%C4%8Dka-716aa5124\">linkedin.com/in/tomáš-růžička-716aa5124</a><br><br>"
		"Running Technology:<br>Qt %2").arg(VERSION).arg(QT_VERSION_STR);


Window::Window(QSettings* settings, Scene::Definitions* definitions) :
	QMainWindow(),
	m_NewFileCounter(1),
	m_Settings(settings),
	m_Definitions(definitions),
	m_Ui(new Ui::Window()),
	m_Status(null),
	m_Progress(null)
{
	ResetClipboard();

	m_Ui->setupUi(this);
	setWindowTitle(QString("%1 %2").arg(windowTitle()).arg(VERSION));

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

	connect(m_Ui->Menu_New,        SIGNAL(triggered()), this, SLOT(NewFile()));
	connect(m_Ui->Menu_Open,       SIGNAL(triggered()), this, SLOT(OpenFile()));
	connect(m_Ui->Menu_OpenRecent, SIGNAL(triggered()), this, SLOT(OpenRecentFile()));
	connect(m_Ui->Menu_Reload,     SIGNAL(triggered()), this, SLOT(ReloadFile()));
	connect(m_Ui->Menu_Save,       SIGNAL(triggered()), this, SLOT(SaveFile()));
	connect(m_Ui->Menu_SaveAs,     SIGNAL(triggered()), this, SLOT(SaveAsFile()));
	connect(m_Ui->Menu_Close,      SIGNAL(triggered()), this, SLOT(CloseFile()));
	connect(m_Ui->Menu_Exit,       SIGNAL(triggered()), this, SLOT(ExitApp()));

	connect(m_Ui->Menu_Cut,        SIGNAL(triggered()), this, SLOT(CutNode()));
	connect(m_Ui->Menu_Copy,       SIGNAL(triggered()), this, SLOT(CopyNode()));
	connect(m_Ui->Menu_Paste,      SIGNAL(triggered()), this, SLOT(PasteNode()));
	connect(m_Ui->Menu_Duplicate,  SIGNAL(triggered()), this, SLOT(DuplicateNode()));
	connect(m_Ui->Menu_Delete,     SIGNAL(triggered()), this, SLOT(DeleteNode()));
	connect(m_Ui->Menu_Export,     SIGNAL(triggered()), this, SLOT(ExportNode()));
	connect(m_Ui->Menu_Import,     SIGNAL(triggered()), this, SLOT(ImportNode()));

	connect(m_Ui->Menu_About,      SIGNAL(triggered()), this, SLOT(ShowAbout()));

	auto  menuCreate      = m_Ui->Menu_Create;
	auto  actionsCreate   = QList<QAction*>();
	auto& nodeDefinitions = m_Definitions->GetNodes();

	foreach (definition, nodeDefinitions)
	{
		if ((definition->Fields.isEmpty() == false && definition->Fields[0].FieldType->Type == Definitions::ENodeFieldType::Unknown) ||
			(definition->Name == "Unknown"))
			continue;

		auto action = new CreateAction(&(*definition), menuCreate);
		connect(action, SIGNAL(triggered()), this, SLOT(CreateNode()));

		actionsCreate.push_back(action);
	}

	menuCreate->addActions(actionsCreate);
}

Window::~Window()
{
	disconnect(m_Ui->Menu_New,        SIGNAL(triggered()), this, SLOT(NewFile()));
	disconnect(m_Ui->Menu_Open,       SIGNAL(triggered()), this, SLOT(OpenFile()));
	disconnect(m_Ui->Menu_OpenRecent, SIGNAL(triggered()), this, SLOT(OpenRecentFile()));
	disconnect(m_Ui->Menu_Reload,     SIGNAL(triggered()), this, SLOT(ReloadFile()));
	disconnect(m_Ui->Menu_Save,       SIGNAL(triggered()), this, SLOT(SaveFile()));
	disconnect(m_Ui->Menu_SaveAs,     SIGNAL(triggered()), this, SLOT(SaveAsFile()));
	disconnect(m_Ui->Menu_Close,      SIGNAL(triggered()), this, SLOT(CloseFile()));
	disconnect(m_Ui->Menu_Exit,       SIGNAL(triggered()), this, SLOT(ExitApp()));

	disconnect(m_Ui->Menu_Cut,        SIGNAL(triggered()), this, SLOT(CutNode()));
	disconnect(m_Ui->Menu_Copy,       SIGNAL(triggered()), this, SLOT(CopyNode()));
	disconnect(m_Ui->Menu_Paste,      SIGNAL(triggered()), this, SLOT(PasteNode()));
	disconnect(m_Ui->Menu_Duplicate,  SIGNAL(triggered()), this, SLOT(DuplicateNode()));
	disconnect(m_Ui->Menu_Delete,     SIGNAL(triggered()), this, SLOT(DeleteNode()));
	disconnect(m_Ui->Menu_Export,     SIGNAL(triggered()), this, SLOT(ExportNode()));
	disconnect(m_Ui->Menu_Import,     SIGNAL(triggered()), this, SLOT(ImportNode()));

	disconnect(m_Ui->Menu_About,      SIGNAL(triggered()), this, SLOT(ShowAbout()));

	auto actionsCreate = m_Ui->Menu_Create->actions();
	foreach (action, actionsCreate)
	{
		disconnect(*action, SIGNAL(triggered()), this, SLOT(CreateNode()));
	}

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

	connect(tab,      SIGNAL(ProgressChanged(float)),                   this, SLOT(UpdateProgress(float)));
	connect(tab,      SIGNAL(EditMenuUpdateRequested(NodeItem*, bool)), this, SLOT(UpdateEditMenu(NodeItem*, bool)));
	connect(tab,      SIGNAL(EditMenuShowRequested(QPoint)),            this, SLOT(ShowEditMenu(QPoint)));
	connect(document, SIGNAL(DirtyStateChanged(bool)),                  this, SLOT(UpdateDirtyState(bool)));

	tab->SetupTree();

	auto tabs = m_Ui->Tabs;

	tabs->addTab(tab, GetFileName(document));
	tabs->setCurrentIndex(tabs->count() - 1);

	m_Ui->Menu_Reload->setEnabled(true);
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

	disconnect(tab,      SIGNAL(ProgressChanged(float)),                   this, SLOT(UpdateProgress(float)));
	disconnect(tab,      SIGNAL(EditMenuUpdateRequested(NodeItem*, bool)), this, SLOT(UpdateEditMenu(NodeItem*, bool)));
	disconnect(tab,      SIGNAL(EditMenuShowRequested(QPoint)),            this, SLOT(ShowEditMenu(QPoint)));
	disconnect(document, SIGNAL(DirtyStateChanged(bool)),                  this, SLOT(UpdateDirtyState(bool)));

	auto tabs = m_Ui->Tabs;
	tabs->removeTab(tabIdx);
	tab->deleteLater();

	if (tabs->count() == 0)
	{
		m_Ui->Menu_Reload->setEnabled(false);
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

	m_Settings->setValue(RECENT_FILE_PATH, file);
}

void Window::OpenRecentFile()
{
	auto file = m_Settings->value(RECENT_FILE_PATH).toString();
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
	auto tab      = GetTab(idx);
	auto document = tab->GetDocument();
	auto result   = SaveIfDialog(document);

	if (result == false)
		return false;

	if (m_ClipboardTab == tab)
	{
		ResetClipboard();
	}

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

void Window::CutNode()
{
	CopyNode();
	m_ClipboardIsCut = true;
}

void Window::CopyNode()
{
	auto tab         = GetCurrentTab();
	m_ClipboardTab   = tab;
	m_ClipboardNode  = tab->GetSelectedSceneNode();
	m_ClipboardIsCut = false;
}

void Window::PasteNode()
{
	if (m_ClipboardNode == null)
		return;

	QBuffer data;
	data.open(QIODevice::ReadWrite);

	SceneNodeSerializer::Serialize(data, *m_ClipboardNode);
	data.seek(0);

	AddNode(data);
}

void Window::DuplicateNode()
{
	CopyNode();
	PasteNode();
}

void Window::CreateNode()
{
	auto action      = as(sender(), CreateAction*);
	auto definition  = action->Definition;

	auto node        = new SceneNode();
	node->Type       = definition->Type;
	node->Definition = definition;

	auto fieldsSize  = SceneNodeUtility::CreateFieldsData(node->Fields, definition->Fields);
	node->Size       = sizeof(node->Type) + sizeof(node->Size) + fieldsSize;

	auto result      = AddNode(node);
	if (result == false)
	{
		delete node;
	}
}

void Window::DeleteNode()
{
	auto tab      = GetCurrentTab();
	auto nodeItem = tab->GetSelectedNode();

	if (m_ClipboardNode == nodeItem->Node)
	{
		ResetClipboard();
	}

	tab->RemoveNode(nodeItem);
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

	AddNode(reader);
}

void Window::ShowAbout()
{
	QMessageBox::about(this, windowTitle(), ABOUT_TEXT);
}

void Window::UpdateEditMenu(int tabIdx)
{
	auto tab          = (tabIdx != -1)   ? GetTab(tabIdx)         : null;
	auto selectedNode = (tab    != null) ? tab->GetSelectedNode() : null;

	UpdateEditMenu(selectedNode);
}

void Window::UpdateEditMenu(NodeItem* nodeItem, bool isFullyDisabled)
{
	auto isNodeSelected = (nodeItem != null);
	auto actions        = m_Ui->Menu_Edit->actions();

	foreach (action, actions)
	{
		(*action)->setEnabled(isNodeSelected);
	}

	if (m_Ui->Tabs->count() > 0 && isFullyDisabled == false)
	{
		m_Ui->Menu_Create->setEnabled(true);
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

void Window::GetAddNodeContext(NodeItem*& parentItem, SceneNode*& parentNode, int& idx)
{
	auto sibling = GetCurrentTab()->GetSelectedNode();
	parentItem   = (sibling != null) ? as(sibling->parent(), NodeItem*) : null;

	if (parentItem == null && sibling != null)
	{
		parentItem = sibling;
	}

	parentNode = null;
	idx        = 0;

	if (parentItem == null)
		return;

	parentNode = parentItem->Node;
	idx        = parentItem->indexOfChild(sibling) + 1;
}

void Window::AddNode(QIODevice& reader)
{
	NodeItem*  parentItem;
	SceneNode* parentNode;
	int        idx;
	GetAddNodeContext(parentItem, parentNode, idx);

	auto node   = SceneNodeSerializer::Deserialize(reader, parentNode, *m_Definitions);
	auto result = GetCurrentTab()->AddNode(node, parentItem, idx);

	if (result == false)
	{
		delete node;
	}
}

bool Window::AddNode(SceneNode* node)
{
	NodeItem*  parentItem;
	SceneNode* parentNode;
	int        idx;
	GetAddNodeContext(parentItem, parentNode, idx);

	return GetCurrentTab()->AddNode(node, parentItem, idx);
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

	m_Settings->setValue(RECENT_FILE_PATH, file);
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

void Window::ResetClipboard()
{
	m_ClipboardTab   = null;
	m_ClipboardNode  = null;
	m_ClipboardIsCut = false;
}
