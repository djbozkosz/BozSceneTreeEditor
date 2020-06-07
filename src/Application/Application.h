#ifndef APPLICATION_APPLICATION_H
#define APPLICATION_APPLICATION_H

#include <QApplication>
#include <QSet>

#include "Utility/Base.h"


class QSettings;


namespace Djbozkosz {
namespace Application {
namespace GUI
{
	class Window;
}


namespace Scene
{
	class Definitions;
}


	class Document;


	sealed class Application : public QApplication
	{
		private:

		Q_OBJECT

		private: // members

		GUI::Window*        m_Window;
		QSet<Document*>     m_Documents;
		QSettings*          m_Settings;

		Scene::Definitions* m_Definitions;

		public: // methods

		explicit Application(int argc, char* argv[]);
		virtual ~Application();

		private slots: // handlers

		void CreateDocument(int idx);
		void LoadDocument(const QString& file);
		void ReloadDocument(Document* document);
		void SaveDocument(Document* document, const QString& file);
		void CloseDocument(Document* document);
	};
}}

#endif // APPLICATION_APPLICATION_H
