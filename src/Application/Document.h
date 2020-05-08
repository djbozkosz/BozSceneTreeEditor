#ifndef APPLICATION_SCENE_DOCUMENT_H
#define APPLICATION_SCENE_DOCUMENT_H

#include <QString>

#include "Utility/Base.h"


namespace Djbozkosz {
namespace Application {
namespace Scene
{
	class SceneTree;
	class SceneNodeDefinitions;
}


	sealed class Document
	{
		private: // members

		Scene::SceneTree* m_Tree;

		public: // methods

		Document(Scene::SceneNodeDefinitions* nodeDefinitions, const QString& file = QString());
		virtual ~Document();
	};
}}

#endif // APPLICATION_SCENE_DOCUMENT_H
