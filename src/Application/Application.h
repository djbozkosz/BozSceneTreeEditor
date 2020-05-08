#ifndef APPLICATION_APPLICATION_H
#define APPLICATION_APPLICATION_H

#include <QApplication>
#include <QMap>

#include "Utility/Base.h"


namespace Djbozkosz {
namespace Application {
namespace GUI
{
	class Window;
}


namespace Scene
{
	class SceneNodeDefinitions;
}


	class Document;


	sealed class Application : public QApplication
	{
		private:

		Q_OBJECT

		private: // members

		GUI::Window*                 m_Window;
		QMap<QString, Document*>     m_Documents;

		Scene::SceneNodeDefinitions* m_NodeDefinitions;

		public: // methods

		Application(int argc, char* argv[]);
		virtual ~Application();

		private slots: // handlers

		void CreateDocument(const QString& file);
		void CloseDocument(const QString& file);
	};
}}

#endif // APPLICATION_APPLICATION_H
