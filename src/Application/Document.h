#ifndef APPLICATION_SCENE_DOCUMENT_H
#define APPLICATION_SCENE_DOCUMENT_H

#include <QObject>
#include <QString>

#include "Utility/Base.h"


namespace Djbozkosz {
namespace Application {
namespace Scene
{
	class SceneTree;
	class Definitions;
}


	sealed class Document : public QObject
	{
		private:

		Q_OBJECT

		private: // members

		QString           m_File;
		Scene::SceneTree* m_Tree;

		bool              m_IsDirty;

		public: // methods

		explicit Document(QObject* parent);
		virtual ~Document();

		void Load(const QString& file, const Scene::Definitions& definitions);
		void Save(const QString& file);

		inline const QString&          GetFile() const { return m_File; }
		inline       Scene::SceneTree* GetTree() const { return m_Tree; }

		inline bool IsDirty() const { return m_IsDirty; }
		void        SetDirty(bool isDirty);

		signals: // interface

		void DirtyStateChanged(bool isDirty);
	};
}}

#endif // APPLICATION_SCENE_DOCUMENT_H
