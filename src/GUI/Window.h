#ifndef APPLICATION_GUI_WINDOW_H
#define APPLICATION_GUI_WINDOW_H

#include <QAction>
#include <QList>
#include <QMainWindow>
#include <QPalette>
#include <QString>

#include "Scene/Definitions.h"


class QLabel;
class QProgressBar;
class QSettings;


namespace Ui
{
	class Window;
}


namespace Djbozkosz {
namespace Application {
namespace Scene
{
	class SceneNode;
}


class Document;


namespace GUI
{
	class NodeItem;
	class DocumentWindow;
	class StartWindow;


	using namespace Djbozkosz::Application::Scene;


	sealed class CreateAction : public QAction
	{
		private:

		Q_OBJECT

		public: // members

		const Definitions::NodeDefinition* Definition;

		public: // methods

		explicit inline CreateAction(const Definitions::NodeDefinition* definition, QObject* parent = null) :
			QAction(parent),
			Definition(definition)
		{
			setText(definition->Name);
		}
	};


	sealed class Window : public QMainWindow
	{
		private:

		Q_OBJECT

		private: // types

		typedef bool (__stdcall *TShouldAppsUseDarkMode)();

		private: // constants

		static const int     MAX_RECENT_FILES;

		static const QString ACTIVE_THEME;
		static const QString OPEN_FILE_DIALOG_PATH;
		static const QString SAVE_FILE_DIALOG_PATH;
		static const QString RECENT_FILE_PATHS;

		static const QString IMPORT_FILE_DIALOG_PATH;
		static const QString EXPORT_FILE_DIALOG_PATH;

		static const QString ABOUT_TEXT;

		private: // members

		StartWindow*           m_StartWindow;

		int                    m_NewFileCounter;

		QSettings*             m_Settings;
		Scene::Definitions*    m_Definitions;

		Ui::Window*            m_Ui;
		QLabel*                m_Status;
		QProgressBar*          m_Progress;

		void*                  m_UxThemeLibrary;
		TShouldAppsUseDarkMode m_ShouldAppsUseDarkMode;

		QPalette               m_DefaultPalette;

		QList<QAction*>        m_RecentFileActions;

		DocumentWindow*        m_ClipboardTab;
		SceneNode*             m_ClipboardNode;
		bool                   m_ClipboardIsCut;

		public: // methods

		explicit Window(QSettings* settings, Scene::Definitions* definitions);
		virtual ~Window();

		void AddDocument(Document* document);
		void RemoveDocumemt(Document* document);

		signals: // public interface

		void FileCreated(int idx);
		void FileOpened(const QString& file);
		void FileReloaded(Document* document);
		void FileSaved(Document* document, const QString& file = QString());
		void FileClosed(Document* document);

		private: // QObject implementation

		override bool eventFilter(QObject* object, QEvent* event);

		private: // QMainWindow implementation

		override void closeEvent(QCloseEvent* event);
		override void dragEnterEvent(QDragEnterEvent *event);
		override void dropEvent(QDropEvent *event);

		public slots: // handlers

		// File
		void NewFile();
		void OpenFile();
		void OpenFile(const QString& file);
		void OpenRecentFile();
		void ReloadFile();
		void SaveFile();
		void SaveAsFile();
		bool SaveIfDialog(Document* document);
		bool CloseFile();
		bool CloseFile(int idx);
		bool ExitApp();

		// Edit
		void CutNode();
		void CopyNode();
		void PasteNode();
		void DuplicateNode();
		void CreateNode();
		void DeleteNode();
		void ExportNode();
		void ImportNode();

		// Window
		void DefaultTheme();
		void DarkTheme();
		void SystemTheme();

		// Help
		void ShowAbout();

		private slots: // handlers

		void UpdateTheme(bool isDarkMode);
		void UpdateEditMenu(int tabIdx);
		void UpdateEditMenu(NodeItem* nodeItem, bool isFullyDisabled = false);
		void ShowEditMenu(QPoint point);
		void UpdateProgress(float value);
		void UpdateDirtyState(bool isDirty);

		private: // methods

		bool IsSystemDarkThemeActive() const;

		DocumentWindow* GetTab(int idx)      const;
		DocumentWindow* GetCurrentTab()      const;
		int             GetCurrentTabIdx()   const;
		Document*       GetCurrentDocument() const;

		void GetAddNodeContext(NodeItem*& parentItem, SceneNode*& parentNode, int& idx);
		void AddNode(QIODevice& reader);
		bool AddNode(SceneNode* node);

		void UpdateDefinitionActions();

		void AddRecentFile(const QString& filename);
		void UpdateRecentFilesMenu();

		void SaveDocument(Document* document, bool replace);

		static QString GetFileName(Document* document, bool useDirtyState = true);

		QString ShowFileDialog(bool isSave, bool isDirectory, const QString& title, const QString& settingsPath, const Definitions::DialogFiles& files = Definitions::DialogFiles(), const QString& file = QString());

		void ResetClipboard();
	};
}}}

#endif // APPLICATION_GUI_WINDOW_H
