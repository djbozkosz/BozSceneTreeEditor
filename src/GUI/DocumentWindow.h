#ifndef APPLICATION_GUI_DOCUMENTWINDOW_H
#define APPLICATION_GUI_DOCUMENTWINDOW_H

#include <QWidget>
#include <QTreeWidgetItem>

#include "ui_DocumentWindow.h"

#include "Scene/Definitions.h"
#include "Scene/SceneNodeUtility.h"


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
	using namespace Djbozkosz::Application::Scene;


	sealed class DocumentWindow : public QWidget
	{											 
		private:

		Q_OBJECT

		private: // types

		sealed class NodeItem : public QTreeWidgetItem
		{
			public:

			SceneNode* Node;

			explicit inline NodeItem(QTreeWidget* parent, SceneNode* node, QString& name) :
				QTreeWidgetItem(parent, QStringList() << name),
				Node(node)
			{
			}

			explicit inline NodeItem(NodeItem* parent, SceneNode* node, QString& name) :
				QTreeWidgetItem(parent, QStringList() << name),
				Node(node)
			{
			}

			virtual ~NodeItem()
			{
			}
		};

		sealed class FieldItem : public QTableWidgetItem
		{
			public:

			NodeItem*                      Item;
			SceneNodeUtility::FieldContext FieldCtx;

			explicit inline FieldItem(const QString& text, NodeItem* item, const SceneNodeUtility::FieldContext& fieldCtx) : QTableWidgetItem(text), Item(item), FieldCtx(fieldCtx) {}
			virtual inline ~FieldItem() {}
		};

		sealed class ReadOnlyItem : public QTableWidgetItem
		{
			public:

			explicit inline ReadOnlyItem(const QString& text) : QTableWidgetItem(text)
			{
				setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			}

			virtual inline ~ReadOnlyItem() {}
		};

		private: // members

		Ui::DocumentWindow* m_Ui;

		Document*           m_Document;
		SceneTree*          m_Tree;
		Definitions*        m_Definitions;


		public: // methods

		explicit DocumentWindow(Document* document, Definitions* definitions, QWidget* parent = null);
		virtual ~DocumentWindow();

		inline Document* GetDocument() const { return m_Document; }

		private: // methods

		void SetupTree();
		void CreateTree(NodeItem* nodeItem, SceneNode* node);
		void UpdateNode(NodeItem* nodeItem);

		void SetupTable(NodeItem* nodeItem);
		void SetupTableField(NodeItem* nodeItem, const SceneNodeUtility::FieldContext& fieldCtx, int& row);

		QString GetEnumValue(const SceneNodeUtility::FieldContext& fieldCtx, int data);
		QString GetNodeName(SceneNode* node) const;

		private slots: // handlers

		void UpdateTable(QTreeWidgetItem* current, QTreeWidgetItem* previous);
		void UpdateField(QTableWidgetItem* item);
	};
}}}

#endif // APPLICATION_GUI_DOCUMENTWINDOW_H
