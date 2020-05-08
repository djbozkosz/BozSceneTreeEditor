#ifndef APPLICATION_GUI_WINDOW_H
#define APPLICATION_GUI_WINDOW_H

#include <QMainWindow>
#include <QString>

#include "Utility/Base.h"


namespace Ui
{
	class Window;
}


namespace Djbozkosz {
namespace Application {
namespace GUI
{
	sealed class Window : public QMainWindow
	{
		private:

		Q_OBJECT

		private: // members

		Ui::Window* m_Ui;

		public: // methods

		Window();
		virtual ~Window();

		signals: // public interface

		void FileOpened(const QString& file);

		private slots: // handlers

		void OpenFile();
		void ExitApp();
	};
}}}

#endif // APPLICATION_GUI_WINDOW_H
