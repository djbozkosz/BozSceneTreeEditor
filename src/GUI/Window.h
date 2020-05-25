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

		Ui::Window*   m_Ui;
		QLabel*       m_Status;
		QProgressBar* m_Progress;

		public: // methods

		explicit Window();
		virtual ~Window();

		void AddDocument(Document* document, Scene::Definitions* definitions);

		signals: // public interface

		void FileCreated();
		void FileOpened(const QString& file);
		void FileSaved(Document* document, const QString& file);

		private slots: // handlers

		void NewFile();
		void OpenFile();
		void SaveFile();
		void SaveAsFile();
		void CloseFile();
		void ExitApp();
		void ShowAbout();

		void UpdateProgress(float value);
		void UpdateDirtyState(bool isDirty);

		private: // methods

		inline DocumentWindow* GetCurrentDocument() const;

		void SaveDocument(bool replace);
	};
}}}

#endif // APPLICATION_GUI_WINDOW_H
