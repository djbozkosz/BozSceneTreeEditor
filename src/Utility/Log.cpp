#include <QMessageBox>

#include "Utility/Log.h"


using namespace Djbozkosz::Application::Utility;


Debug::Log::Log(EType type, bool condition) : Type(type), m_Condition(condition)
{
}

Debug::Log::~Log()
{
	QtMsgType qLogType;

	switch (Type)
	{
		case EType::Warning:   qLogType = QtWarningMsg;  break;
		case EType::Error:     qLogType = QtCriticalMsg; break;
		case EType::Exception: qLogType = QtCriticalMsg; break;
		default:               qLogType = QtDebugMsg;    break;
	}

	if (Type >= EType::Error)
	{
		QString title;

		switch (Type)
		{
			case EType::Error:     title = "Error";     break;
			case EType::Assert:    title = "Assert";    break;
			case EType::Exception: title = "Exception"; break;
			default:                                    break;
		}

		QMessageBox::critical(null, title, m_Message);
	}

	if (Type == EType::Assert)
	{
		if (m_Condition == false)
		{
			__debugbreak();
			qt_assert(m_Message.toLocal8Bit().constData(), "", 0);
		}
		return;
	}

	qt_message_output(qLogType, QMessageLogContext(), m_Message.toLocal8Bit().constData());
}
