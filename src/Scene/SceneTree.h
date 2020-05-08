#ifndef APPLICATION_SCENE_SCENETREE_H
#define APPLICATION_SCENE_SCENETREE_H

#include <QString>

#include "Utility/Base.h"


namespace Djbozkosz {
namespace Application {
namespace Scene
{
	sealed class SceneTree
	{
		private: // members

		public: // methods

		SceneTree(const QString& file = QString());
		virtual ~SceneTree();

		private: // methods

		bool Load(const QString& file);
		bool Save(const QString& file);
	};
}}}

#endif // APPLICATION_SCENE_SCENETREE_H
