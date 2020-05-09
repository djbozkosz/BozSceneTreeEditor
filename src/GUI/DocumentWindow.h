#ifndef APPLICATION_GUI_DOCUMENTWINDOW_H
#define APPLICATION_GUI_DOCUMENTWINDOW_H

#include <QWidget>
#include <QTreeWidgetItem>

#include "Utility/Base.h"


class QTreeWidget;
class QTreeWidgetItem;


namespace Ui
{
	class DocumentWindow;
}


namespace Djbozkosz {
namespace Application
{
class Document;


namespace Scene
{
	class SceneNode;
	class SceneTree;
}


namespace GUI
{
	sealed class DocumentWindow : public QWidget
	{											 
		private:

		Q_OBJECT

		private: // types

		sealed class NodeItem : public QTreeWidgetItem
		{
			public:

			Scene::SceneNode* Node;

			explicit NodeItem(QTreeWidget* parent, Scene::SceneNode* node);
			explicit NodeItem(NodeItem* parent, Scene::SceneNode* node);
			virtual ~NodeItem();

			private:

			static QString GetName(Scene::SceneNode* node);
		};

		private: // members

		Ui::DocumentWindow* m_Ui;

		Document*           m_Document;
		Scene::SceneTree*   m_Tree;


		public: // methods

		explicit DocumentWindow(Document* document, QWidget* parent = null);
		virtual ~DocumentWindow();

		private: // methods

		void SetupTree();
		static void CreateTree(NodeItem *item, Scene::SceneNode* node);

		void SetupTable(Scene::SceneNode* node);

		private slots: // handlers

		void UpdateTable(QTreeWidgetItem* current, QTreeWidgetItem* previous);
	};
}}}

#endif // APPLICATION_GUI_DOCUMENTWINDOW_H
