#include <QBuffer>
#include <QCloseEvent>
#include <QDragEnterEvent>
#include <QEvent>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QLabel>
#include <QList>
#include <QMainWindow>
#include <QMenu>
#include <QMessageBox>
#include <QMimeData>
#include <QProgressBar>
#include <QPushButton>
#include <QSettings>
#include <QVariant>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#include "ui_Window.h"

#include "GUI/DocumentWindow.h"
#include "GUI/StartWindow.h"
#include "GUI/Window.h"
#include "Application/Document.h"
#include "Scene/SceneNode.h"
#include "Scene/SceneNodeSerializer.h"


using namespace Djbozkosz::Application::GUI;


const int     Window::MAX_RECENT_FILES        = 15;

const QString Window::ACTIVE_THEME            = "ActiveTheme";
const QString Window::OPEN_FILE_DIALOG_PATH   = "OpenFileDialogPath";
const QString Window::SAVE_FILE_DIALOG_PATH   = "SaveFileDialogPath";
const QString Window::RECENT_FILE_PATHS       = "RecentFilePaths";

const QString Window::IMPORT_FILE_DIALOG_PATH = "ImportFileDialogPath";
const QString Window::EXPORT_FILE_DIALOG_PATH = "ExportFileDialogPath";

const QString Window::ABOUT_TEXT =
		QString("After short development<br>created by: djbozkosz<br><br>"
		"Version: %1<br>"
		"Copyright (c): 2005 - 2026<br><br>"
		"Found a bug or have some ideas how to improve this app?<br>Don't hesitate to contact me at <a href=\"mailto:t_ruzicka@email.cz\">t_ruzicka@email.cz</a>.<br><br>"
		"Web: <a href=\"http://www.djbozkosz.wz.cz\">www.djbozkosz.wz.cz</a><br>"
		"Facebook: <a href=\"https://www.facebook.com/tomas.ruzicka.73\">facebook.com/tomas.ruzicka.73</a><br>"
		"Twitter: <a href=\"https://twitter.com/djbozkosz\">twitter.com/djbozkosz</a><br>"
		"GitHub: <a href=\"https://www.github.com/djbozkosz\">github.com/djbozkosz</a><br>"
		"LinkedIn: <a href=\"https://www.linkedin.com/in/tom%C3%A1%C5%A1-r%C5%AF%C5%BEi%C4%8Dka-716aa5124\">linkedin.com/in/tomáš-růžička-716aa5124</a><br><br>"
		"Running Technology:<br>Qt %2").arg(VERSION).arg(QT_VERSION_STR);


Window::Window(QSettings* settings, Scene::Definitions* definitions) :
	QMainWindow(),
	m_StartWindow(null),
	m_NewFileCounter(1),
	m_Settings(settings),
	m_Definitions(definitions),
	m_Ui(new Ui::Window()),
	m_Status(null),
	m_Progress(null)
{
	m_DefaultPalette = qApp->palette();

	ResetClipboard();

	m_Ui->setupUi(this);
	setWindowTitle(QString("%1 v%2").arg(windowTitle()).arg(VERSION));

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

	connect(m_Ui->Menu_New,          SIGNAL(triggered()), this, SLOT(NewFile()));
	connect(m_Ui->Menu_Open,         SIGNAL(triggered()), this, SLOT(OpenFile()));
	connect(m_Ui->Menu_OpenRecent,   SIGNAL(triggered()), this, SLOT(OpenRecentFile()));
	connect(m_Ui->Menu_Reload,       SIGNAL(triggered()), this, SLOT(ReloadFile()));
	connect(m_Ui->Menu_Save,         SIGNAL(triggered()), this, SLOT(SaveFile()));
	connect(m_Ui->Menu_SaveAs,       SIGNAL(triggered()), this, SLOT(SaveAsFile()));
	connect(m_Ui->Menu_Close,        SIGNAL(triggered()), this, SLOT(CloseFile()));
	connect(m_Ui->Menu_Exit,         SIGNAL(triggered()), this, SLOT(ExitApp()));

	connect(m_Ui->Menu_DefaultTheme, SIGNAL(triggered()), this, SLOT(DefaultTheme()));
	connect(m_Ui->Menu_DarkTheme,    SIGNAL(triggered()), this, SLOT(DarkTheme()));
	connect(m_Ui->Menu_SystemTheme,  SIGNAL(triggered()), this, SLOT(SystemTheme()));

	connect(m_Ui->Menu_Cut,          SIGNAL(triggered()), this, SLOT(CutNode()));
	connect(m_Ui->Menu_Copy,         SIGNAL(triggered()), this, SLOT(CopyNode()));
	connect(m_Ui->Menu_Paste,        SIGNAL(triggered()), this, SLOT(PasteNode()));
	connect(m_Ui->Menu_Duplicate,    SIGNAL(triggered()), this, SLOT(DuplicateNode()));
	connect(m_Ui->Menu_Delete,       SIGNAL(triggered()), this, SLOT(DeleteNode()));
	connect(m_Ui->Menu_Export,       SIGNAL(triggered()), this, SLOT(ExportNode()));
	connect(m_Ui->Menu_Import,       SIGNAL(triggered()), this, SLOT(ImportNode()));

	connect(m_Ui->Menu_About,        SIGNAL(triggered()), this, SLOT(ShowAbout()));

	UpdateRecentFilesMenu();

	UpdateDefinitionActions();

	m_StartWindow = new StartWindow(m_Settings, this, m_Ui->Container);
	m_StartWindow->installEventFilter(this);

	m_Ui->ContainerVerticalLayout->insertWidget(0, m_StartWindow);
	m_Ui->Tabs->hide();

#ifdef Q_OS_WIN
	auto uxThemeLibrary = LoadLibraryA("uxtheme.dll");
	if (uxThemeLibrary != null)
	{
		m_ShouldAppsUseDarkMode = reinterpret_cast<TShouldAppsUseDarkMode>(GetProcAddress(uxThemeLibrary, MAKEINTRESOURCEA(132)));
	}
	else
	{
		m_ShouldAppsUseDarkMode = null;
	}
	m_UxThemeLibrary = uxThemeLibrary;
#else
	m_UxThemeLibrary        = null;
	m_ShouldAppsUseDarkMode = null;
#endif

	auto activeTheme = m_Settings->value(ACTIVE_THEME, -1);
	if (activeTheme == 0)
	{
		DefaultTheme();
	}
	else if (activeTheme == 1)
	{
		DarkTheme();
	}
	else
	{
		SystemTheme();
	}
}

Window::~Window()
{
	disconnect(m_Ui->Menu_New,          SIGNAL(triggered()), this, SLOT(NewFile()));
	disconnect(m_Ui->Menu_Open,         SIGNAL(triggered()), this, SLOT(OpenFile()));
	disconnect(m_Ui->Menu_OpenRecent,   SIGNAL(triggered()), this, SLOT(OpenRecentFile()));
	disconnect(m_Ui->Menu_Reload,       SIGNAL(triggered()), this, SLOT(ReloadFile()));
	disconnect(m_Ui->Menu_Save,         SIGNAL(triggered()), this, SLOT(SaveFile()));
	disconnect(m_Ui->Menu_SaveAs,       SIGNAL(triggered()), this, SLOT(SaveAsFile()));
	disconnect(m_Ui->Menu_Close,        SIGNAL(triggered()), this, SLOT(CloseFile()));
	disconnect(m_Ui->Menu_Exit,         SIGNAL(triggered()), this, SLOT(ExitApp()));

	disconnect(m_Ui->Menu_DefaultTheme, SIGNAL(triggered()), this, SLOT(DefaultTheme()));
	disconnect(m_Ui->Menu_DarkTheme,    SIGNAL(triggered()), this, SLOT(DarkTheme()));
	disconnect(m_Ui->Menu_SystemTheme,  SIGNAL(triggered()), this, SLOT(SystemTheme()));

	disconnect(m_Ui->Menu_Cut,          SIGNAL(triggered()), this, SLOT(CutNode()));
	disconnect(m_Ui->Menu_Copy,         SIGNAL(triggered()), this, SLOT(CopyNode()));
	disconnect(m_Ui->Menu_Paste,        SIGNAL(triggered()), this, SLOT(PasteNode()));
	disconnect(m_Ui->Menu_Duplicate,    SIGNAL(triggered()), this, SLOT(DuplicateNode()));
	disconnect(m_Ui->Menu_Delete,       SIGNAL(triggered()), this, SLOT(DeleteNode()));
	disconnect(m_Ui->Menu_Export,       SIGNAL(triggered()), this, SLOT(ExportNode()));
	disconnect(m_Ui->Menu_Import,       SIGNAL(triggered()), this, SLOT(ImportNode()));

	disconnect(m_Ui->Menu_About,        SIGNAL(triggered()), this, SLOT(ShowAbout()));

	auto actionsCreate = m_Ui->Menu_Create->actions();
	foreach (action, actionsCreate)
	{
		disconnect(*action, SIGNAL(triggered()), this, SLOT(CreateNode()));
	}

	auto tabs = m_Ui->Tabs;
	tabs->removeEventFilter(this);
	disconnect(tabs, SIGNAL(currentChanged(int)),    this, SLOT(UpdateEditMenu(int)));
	disconnect(tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(CloseFile(int)));

	foreach (action, m_RecentFileActions)
	{
		disconnect(*action, SIGNAL(triggered()), this, SLOT(OpenRecentFile()));
	}

	m_RecentFileActions.clear();

	m_StartWindow->removeEventFilter(this);
	m_StartWindow->Cleanup();

	delete m_Ui;

#ifdef Q_OS_WIN
	HMODULE uxThemeLibrary = reinterpret_cast<HMODULE>(m_UxThemeLibrary);
	if (uxThemeLibrary != null)
	{
		FreeLibrary(uxThemeLibrary);
	}
#endif
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

	m_StartWindow->hide();
	tabs->show();

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

		tabs->hide();
		m_StartWindow->show();
	}
}

bool Window::eventFilter(QObject* object, QEvent* event)
{
	if ((object == m_Ui->Tabs || object == m_StartWindow) && event->type() == QEvent::MouseButtonDblClick)
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

void Window::dragEnterEvent(QDragEnterEvent *event)
{
	event->setDropAction(Qt::CopyAction);
	event->accept();
}

void Window::dropEvent(QDropEvent *event)
{
	auto urls = event->mimeData()->urls();

	foreach (url, urls)
	{
		OpenFile(url->toLocalFile());
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

	OpenFile(file);
}

void Window::OpenFile(const QString& file)
{
	emit FileOpened(file);

	AddRecentFile(file);
}

void Window::OpenRecentFile()
{
	auto file = QString();

	auto action = qobject_cast<QAction*>(sender());
	if (action != null)
	{
		file = action->text();
	}

	if (QFileInfo(file).isFile() == false)
	{
		auto pushButton = qobject_cast<QPushButton*>(sender());
		if (pushButton != null)
		{
			file = pushButton->text();
		}
	}

	if (QFileInfo(file).isFile() == false)
	{
		if (m_RecentFileActions.isEmpty() == false)
		{
			file = m_RecentFileActions[0]->text();
		}
	}

	if (QFileInfo(file).isFile() == false)
		return;

	OpenFile(file);
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

void Window::DefaultTheme()
{
	m_Ui->Menu_DefaultTheme->setChecked(true);
	m_Ui->Menu_DarkTheme   ->setChecked(false);
	m_Ui->Menu_SystemTheme ->setChecked(false);

	m_Settings->setValue(ACTIVE_THEME, 0);

	UpdateTheme(false);
}

void Window::DarkTheme()
{
	m_Ui->Menu_DefaultTheme->setChecked(false);
	m_Ui->Menu_DarkTheme   ->setChecked(true);
	m_Ui->Menu_SystemTheme ->setChecked(false);

	m_Settings->setValue(ACTIVE_THEME, 1);

	UpdateTheme(true);
}

void Window::SystemTheme()
{
	m_Ui->Menu_DefaultTheme->setChecked(false);
	m_Ui->Menu_DarkTheme   ->setChecked(false);
	m_Ui->Menu_SystemTheme ->setChecked(true);

	m_Settings->setValue(ACTIVE_THEME, -1);

	UpdateTheme(IsSystemDarkThemeActive());
}

void Window::ShowAbout()
{
	QMessageBox::about(this, windowTitle(), ABOUT_TEXT);
}

void Window::UpdateTheme(bool isDarkMode)
{
	if (isDarkMode == true)
	{
		/*
		* stylesheet setup:

		* background hue:        210
		* background saturation:   0
		* background brightness:  46
		* background depth:       10

		* highlight  hue:        210
		* highlight  saturation: 255
		* highlight  brightness: 160
		* highlight  depth:       24

		* font       hue:          0
		* font       saturation:   0
		* font       brightness: 255
		* font       size:         8
		*/

		auto palette = m_DefaultPalette;
		palette.setColor(QPalette::Window,          QColor("#2e2e2e"));
		palette.setColor(QPalette::WindowText,      QColor("#ffffff"));
		palette.setColor(QPalette::Button,          QColor("#2e2e2e"));
		palette.setColor(QPalette::Light,           QColor("#003383"));
		palette.setColor(QPalette::Midlight,        QColor("#1c6cbc"));
		palette.setColor(QPalette::Dark,            QColor("#424242"));
		palette.setColor(QPalette::Mid,             QColor("#333333"));
		palette.setColor(QPalette::Text,            QColor("#ffffff"));
		palette.setColor(QPalette::BrightText,      QColor("#000000"));
		palette.setColor(QPalette::ButtonText,      QColor("#ffffff"));
		palette.setColor(QPalette::Base,            QColor("#2e2e2e"));
		palette.setColor(QPalette::Shadow,          QColor("#424242"));
		palette.setColor(QPalette::Highlight,       QColor("#3399ff"));
		palette.setColor(QPalette::HighlightedText, QColor("#ffffff"));
		palette.setColor(QPalette::Link,            QColor("#3399ff"));
		palette.setColor(QPalette::LinkVisited,     QColor("#3399ff"));
		palette.setColor(QPalette::AlternateBase,   QColor("#333333"));
		qApp->setPalette(palette);

		QFile stylesheetResource(":/stylesheet/stylesheet.css");
		stylesheetResource.open(QIODevice::ReadOnly);
		auto stylesheet = QString::fromUtf8(stylesheetResource.readAll());
		qApp->setStyleSheet(stylesheet);
	}
	else
	{
		qApp->setPalette(m_DefaultPalette);
		qApp->setStyleSheet(QString());
	}

	m_StartWindow->UpdateTheme(isDarkMode);
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

bool Window::IsSystemDarkThemeActive() const
{
	auto isDark = false;

	if (m_ShouldAppsUseDarkMode != null)
	{
		isDark = m_ShouldAppsUseDarkMode();
	}

	if (isDark == true)
		return true;

	isDark = (m_DefaultPalette.window().color().value() < 128);

	return isDark;
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

void Window::UpdateDefinitionActions()
{
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

void Window::AddRecentFile(const QString& filename)
{
	auto file = filename;
	file.replace("/", "\\");

	auto recentFiles = m_Settings->value(RECENT_FILE_PATHS).toStringList();

	while (true)
	{
		auto recentFileIdx = recentFiles.indexOf(file);
		if (recentFileIdx == -1)
			break;

		recentFiles.removeAt(recentFileIdx);
	}

	recentFiles.push_front(file);

	while (recentFiles.size() > MAX_RECENT_FILES)
	{
		recentFiles.pop_back();
	}

	m_Settings->setValue(RECENT_FILE_PATHS, recentFiles);

	UpdateRecentFilesMenu();
	m_StartWindow->UpdateRecentFilesMenu();
}

void Window::UpdateRecentFilesMenu()
{
	auto recentFiles = m_Settings->value(RECENT_FILE_PATHS).toStringList();

	foreach (action, m_RecentFileActions)
	{
		disconnect(*action, SIGNAL(triggered()), this, SLOT(OpenRecentFile()));

		m_Ui->Menu_File->removeAction(*action);
		(*action)->deleteLater();
	}

	m_RecentFileActions.clear();

	foreach (file, recentFiles)
	{
		auto action = new QAction(*file, m_Ui->Menu_File);
		connect(action, SIGNAL(triggered()), this, SLOT(OpenRecentFile()));

		m_Ui->Menu_File->insertAction(m_Ui->Menu_Exit, action);
		m_RecentFileActions.push_back(action);
	}

	m_Ui->Menu_File->insertSeparator(m_Ui->Menu_Exit);
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

	AddRecentFile(file);
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
