#ifndef APPLICATION_UTILITY_LOG_H
#define APPLICATION_UTILITY_LOG_H

#include "Utility/PreBase.h"

#include <QString>

#include "Utility/Base.h"


namespace Djbozkosz {
namespace Application {
namespace Utility
{
	static_ class Debug
	{
		public: // types

		enum class EType
		{
			Log = 0,
			Warning,
			Error,
			Assert,
			Exception
		};

		class Log
		{
			protected: // members

			EType Type;

			private: // members

			bool    m_Condition;
			QString m_Message;

			public: // methods

			explicit Log(EType type = EType::Log, bool condition = true);
			virtual ~Log();

			inline Log& operator<<(QChar                value) { m_Message += QString("\'%1\'").arg(value);                              return TryPutSpace(); }
			inline Log& operator<<(bool                 value) { m_Message += ((value == true) ? "true" : "false");                      return TryPutSpace(); }
			inline Log& operator<<(char                 value) { m_Message += QString::number(value);                                    return TryPutSpace(); }
			inline Log& operator<<(signed   short       value) { m_Message += QString::number(value);                                    return TryPutSpace(); }
			inline Log& operator<<(unsigned short       value) { m_Message += QString::number(value);                                    return TryPutSpace(); }
			inline Log& operator<<(signed   int         value) { m_Message += QString::number(value);                                    return TryPutSpace(); }
			inline Log& operator<<(unsigned int         value) { m_Message += QString::number(value);                                    return TryPutSpace(); }
			inline Log& operator<<(signed   long        value) { m_Message += QString::number(value);                                    return TryPutSpace(); }
			inline Log& operator<<(unsigned long        value) { m_Message += QString::number(value);                                    return TryPutSpace(); }
			inline Log& operator<<(qint64               value) { m_Message += QString::number(value);                                    return TryPutSpace(); }
			inline Log& operator<<(quint64              value) { m_Message += QString::number(value);                                    return TryPutSpace(); }
			inline Log& operator<<(float                value) { m_Message += QString::number(value);                                    return TryPutSpace(); }
			inline Log& operator<<(double               value) { m_Message += QString::number(value);                                    return TryPutSpace(); }
			inline Log& operator<<(const char*          value) { m_Message += QString::fromLatin1(value);                                return TryPutSpace(); }
			inline Log& operator<<(const QString&       value) { m_Message += "\""+value+"\"";                                           return TryPutSpace(); }
			inline Log& operator<<(const QStringRef&    value) { m_Message += "\""+value.toString()+"\"";                                return TryPutSpace(); }
			inline Log& operator<<(const QLatin1String& value) { m_Message += "\""+QString(value)+"\"";                                  return TryPutSpace(); }
			inline Log& operator<<(const QByteArray&    value) { m_Message += "\""+value+"\"";                                           return TryPutSpace(); }
			inline Log& operator<<(const void*          value) { m_Message += "0x"+QString::number(reinterpret_cast<size_t>(value), 16); return TryPutSpace(); }

			private: // methods

			inline Log& TryPutSpace()
			{
				int length = m_Message.length();

				if (length > 0 && m_Message[length - 1] != ' ')
				{
					m_Message += " ";
				}

				return *this;
			}
		};

		sealed class Warning : public Log
		{
			public: // methods

			explicit inline Warning() : Log(EType::Warning) {}
			virtual inline ~Warning() {}
		};

		sealed class Error : public Log
		{
			public: // methods

			explicit inline Error() : Log(EType::Error) {}
			virtual inline ~Error() {}
		};

		sealed class Assert : public Log
		{
			public: // methods

			explicit inline Assert(bool condition) : Log(EType::Assert, condition) {}
			virtual inline ~Assert() {}
		};

		sealed class Exception : public Log
		{
			public: // methods

			explicit inline Exception() : Log(EType::Exception) {}
			virtual inline ~Exception() {}
		};
	};
}}}

#endif // APPLICATION_UTILITY_LOG_H
