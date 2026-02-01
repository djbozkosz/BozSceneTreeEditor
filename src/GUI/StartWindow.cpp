#include "Utility/PreBase.h"

#include <QPixmap>
#include <QPushButton>
#include <QSettings>

#include "ui_StartWindow.h"

#include "StartWindow.h"
#include "Window.h"


using namespace Djbozkosz::Application::GUI;


const QString StartWindow::RECENT_FILE_PATHS = "RecentFilePaths";


StartWindow::StartWindow(QSettings* settings, Window* window, QWidget* parent) :
	QWidget(parent),
	m_Settings(settings),
	m_Window(window),
	m_Ui(new Ui::StartWindow)
{
	m_Ui->setupUi(this);

	connect(m_Ui->Action_New,        SIGNAL(clicked()), m_Window, SLOT(NewFile()));
	connect(m_Ui->Action_Open,       SIGNAL(clicked()), m_Window, SLOT(OpenFile()));
	connect(m_Ui->Action_OpenRecent, SIGNAL(clicked()), m_Window, SLOT(OpenRecentFile()));

	UpdateRecentFilesMenu();
}

StartWindow::~StartWindow()
{
	delete m_Ui;
}

void StartWindow::Cleanup()
{
	disconnect(m_Ui->Action_New,        SIGNAL(clicked()), m_Window, SLOT(NewFile()));
	disconnect(m_Ui->Action_Open,       SIGNAL(clicked()), m_Window, SLOT(OpenFile()));
	disconnect(m_Ui->Action_OpenRecent, SIGNAL(clicked()), m_Window, SLOT(OpenRecentFile()));

	foreach (action, m_RecentFileActions)
	{
		disconnect(*action, SIGNAL(clicked()), m_Window, SLOT(OpenRecentFile()));
	}

	m_RecentFileActions.clear();
}

void StartWindow::UpdateTheme(bool isDarkMode)
{
	m_Ui->Background ->setProperty("isDarkMode", isDarkMode);
	m_Ui->ActionsArea->setProperty("isDarkMode", isDarkMode);
	m_Ui->InfoArea   ->setProperty("isDarkMode", isDarkMode);

	m_Ui->Background ->style()->unpolish(m_Ui->Background);
	m_Ui->ActionsArea->style()->unpolish(m_Ui->ActionsArea);
	m_Ui->InfoArea   ->style()->unpolish(m_Ui->InfoArea);

	m_Ui->Background ->style()->polish(m_Ui->Background);
	m_Ui->ActionsArea->style()->polish(m_Ui->ActionsArea);
	m_Ui->InfoArea   ->style()->polish(m_Ui->InfoArea);

	m_Ui->GetStartedImage->setPixmap(QPixmap(isDarkMode ? ":/images/image01.png" : ":/images/image00.png"));
}

void StartWindow::UpdateRecentFilesMenu()
{
	auto recentFiles = m_Settings->value(RECENT_FILE_PATHS).toStringList();

	foreach (action, m_RecentFileActions)
	{
		disconnect(*action, SIGNAL(clicked()), m_Window, SLOT(OpenRecentFile()));

		m_Ui->RecentFilesListVerticalLayout->removeWidget(*action);
		(*action)->deleteLater();
	}

	m_RecentFileActions.clear();

	auto templateAction = m_Ui->Action_New;

	foreach (file, recentFiles)
	{
		auto action = new QPushButton(*file, m_Ui->RecentFilesList);
		action->setMinimumHeight(templateAction->minimumHeight());
		action->setCursor       (templateAction->cursor());
		action->setStyleSheet   (templateAction->styleSheet());
		action->setFlat         (templateAction->isFlat());

		connect(action, SIGNAL(clicked()), m_Window, SLOT(OpenRecentFile()));

		m_Ui->RecentFilesListVerticalLayout->insertWidget(m_Ui->RecentFilesListVerticalLayout->count(), action);
		m_RecentFileActions.push_back(action);
	}
}
