#ifndef APPLICATION_SCENE_SCENENODE_H
#define APPLICATION_SCENE_SCENENODE_H

#include "Utility/Base.h"


class QFile;


namespace Djbozkosz {
namespace Application {
namespace Scene
{
	sealed class SceneNode
	{
		private: // members

		public: // methods

		SceneNode();
		virtual ~SceneNode();

		private: // methods

		bool Load(const QFile& reader);
		bool Save(const QFile& writer);
	};
}}}

#endif // APPLICATION_SCENE_SCENENODE_H
