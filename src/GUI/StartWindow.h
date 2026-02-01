#ifndef APPLICATION_GUI_STARTWINDOW_H
#define APPLICATION_GUI_STARTWINDOW_H

#include <QList>
#include <QWidget>

#include "Utility/Base.h"


class QPushButton;
class QSettings;


namespace Ui
{
	class StartWindow;
}


namespace Djbozkosz {
namespace Application {
namespace GUI
{
	class Window;


	sealed class StartWindow : public QWidget
	{
		private:

		Q_OBJECT

		private: // constants

		static const QString RECENT_FILE_PATHS;

		private: // members

		QSettings*          m_Settings;
		Window*             m_Window;

		Ui::StartWindow*    m_Ui;

		QList<QPushButton*> m_RecentFileActions;

		public: // methods

		explicit StartWindow(QSettings* settings, Window* window, QWidget* parent = null);
		virtual ~StartWindow();

		void Cleanup();

		void UpdateTheme(bool isDarkMode);

		void UpdateRecentFilesMenu();
	};
}}}

#endif // APPLICATION_GUI_STARTWINDOW_H
