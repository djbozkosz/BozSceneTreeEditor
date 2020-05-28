#ifndef APPLICATION_GUI_WINDOW_H
#define APPLICATION_GUI_WINDOW_H

#include <QMainWindow>
#include <QString>

#include "Utility/Base.h"


class QLabel;
class QProgressBar;


namespace Ui
{
	class Window;
}


namespace Djbozkosz {
namespace Application {
namespace Scene
{
	class Definitions;
}


class Document;


namespace GUI
{
	class DocumentWindow;


	sealed class Window : public QMainWindow
	{
		private:

		Q_OBJECT

		private: // members

		int           m_NewFileCounter;

		Ui::Window*   m_Ui;
		QLabel*       m_Status;
		QProgressBar* m_Progress;

		public: // methods

		explicit Window();
		virtual ~Window();

		void AddDocument(Document* document, Scene::Definitions* definitions);
		void RemoveDocumemt(Document* document);

		signals: // public interface

		void FileCreated(int idx);
		void FileOpened(const QString& file);
		void FileSaved(Document* document, const QString& file = QString());
		void FileClosed(Document* document);

		private: // QObject implementation

		bool eventFilter(QObject* object, QEvent* event);

		private: // QMainWindow implementation

		void closeEvent(QCloseEvent* event);

		private slots: // handlers

		void NewFile();
		void OpenFile();
		void SaveFile();
		void SaveAsFile();
		bool CloseFile();
		bool CloseFile(int idx);
		bool ExitApp();
		void ShowAbout();

		void UpdateProgress(float value);
		void UpdateDirtyState(bool isDirty);

		private: // methods

		DocumentWindow* GetTab(int idx) const;
		DocumentWindow* GetCurrentTab() const;

		void SaveDocument(DocumentWindow* tab, bool replace);

		static QString GetFileName(Document* document, bool useDirtyState = true);
	};
}}}

#endif // APPLICATION_GUI_WINDOW_H
