#ifndef APPLICATION_SCENE_SCENETREE_H
#define APPLICATION_SCENE_SCENETREE_H

#include <QString>

#include "Utility/Base.h"


namespace Djbozkosz {
namespace Application {
namespace Scene
{
	class SceneNodeDefinitions;
	class SceneNode;


	sealed class SceneTree
	{
		private: // members

		SceneNodeDefinitions* m_NodeDefinitions;
		SceneNode*            m_Root;

		public: // methods

		SceneTree(SceneNodeDefinitions* nodeDefinitions, const QString& file = QString());
		virtual ~SceneTree();

		private: // methods

		void DeleteTree();

		bool Load(const QString& file);
		bool Save(const QString& file) const;
	};
}}}

#endif // APPLICATION_SCENE_SCENETREE_H
