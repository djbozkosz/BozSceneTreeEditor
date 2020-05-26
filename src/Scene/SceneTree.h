#ifndef APPLICATION_SCENE_SCENETREE_H
#define APPLICATION_SCENE_SCENETREE_H

#include <QString>

#include "Utility/Base.h"


class QFile;


namespace Djbozkosz {
namespace Application {
namespace Scene
{
	class Definitions;
	class SceneNode;


	sealed class SceneTree
	{
		public: // members

		SceneNode* Root;

		private: // members

		public: // methods

		explicit SceneTree();
		virtual ~SceneTree();

		void Load(QFile& reader, const Definitions& definitions);
		void Save(QFile& writer) const;

		private: // methods

		void DeleteTree();
	};
}}}

#endif // APPLICATION_SCENE_SCENETREE_H
