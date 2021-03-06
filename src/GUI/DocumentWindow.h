#ifndef APPLICATION_GUI_DOCUMENTWINDOW_H
#define APPLICATION_GUI_DOCUMENTWINDOW_H

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QTableWidgetItem>
#include <QPoint>

#include "Scene/Definitions.h"
#include "Scene/SceneNodeUtility.h"


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
}


namespace GUI
{
	using namespace Djbozkosz::Application::Scene;


	sealed class NodeItem : public QTreeWidgetItem
	{
		public: // members

		SceneNode* Node;

		public: // methods

		explicit inline NodeItem(QTreeWidget* parent, SceneNode* node, const QString& name) :
			QTreeWidgetItem(parent, QStringList() << name),
			Node(node)
		{
		}

		explicit inline NodeItem(NodeItem* parent, SceneNode* node, const QString& name) :
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
		public: // members

		NodeItem*                      Item;
		SceneNodeUtility::FieldContext FieldCtx;

		public: // methods

		explicit inline FieldItem(const QString& text, NodeItem* item, const SceneNodeUtility::FieldContext& fieldCtx) : QTableWidgetItem(text), Item(item), FieldCtx(fieldCtx) {}
		virtual inline ~FieldItem() {}
	};


	sealed class ReadOnlyItem : public QTableWidgetItem
	{
		public: // methods

		explicit inline ReadOnlyItem(const QString& text) : QTableWidgetItem(text)
		{
			setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		}

		virtual inline ~ReadOnlyItem() {}
	};


	sealed class TreeWidget : public QTreeWidget
	{
		private:

		Q_OBJECT

		private: // members

		Document* m_Document;
		NodeItem* m_DraggedNode;

		public: // methods

		explicit TreeWidget(QWidget* parent = null);
		virtual ~TreeWidget();

		inline void SetDocument(Document* document) { m_Document = document; }

		private: // QWidget implementation

		override void dragEnterEvent(QDragEnterEvent* event);
		override void dropEvent(QDropEvent* event);
	};


	sealed class DocumentWindow : public QWidget
	{											 
		private:

		Q_OBJECT

		private: // members

		Ui::DocumentWindow* m_Ui;

		Document*           m_Document;
		Definitions*        m_Definitions;

		QAction*            m_FilterClearAction;
		NodeItem*           m_FilterNodeItemBackup;

		public: // methodssetup

		explicit DocumentWindow(Document* document, Definitions* definitions, QWidget* parent = null);
		virtual ~DocumentWindow();

		void SetupTree();

		bool AddNode(SceneNode* node, NodeItem* parentItem, int idx);
		void RemoveNode(NodeItem* nodeItem);

		inline Document* GetDocument()          const { return m_Document; }
		NodeItem*        GetSelectedNode()      const;
		SceneNode*       GetSelectedSceneNode() const;

		signals: // interface

		void ProgressChanged(float value);
		void EditMenuUpdateRequested(NodeItem* nodeItem, bool isFullyDisabled = false);
		void EditMenuShowRequested(QPoint point);

		private: // methods

		void CreateTree(NodeItem* nodeItem, SceneNode* node, float& progress);
		void UpdateNode(NodeItem* nodeItem);

		void SetupTable(NodeItem* nodeItem);
		void SetupTableField(NodeItem* nodeItem, const SceneNodeUtility::FieldContext& fieldCtx, int& row);

		void UpdateProgress(SceneNode* node, float& progress);
		void SwitchEditMode(bool textChanged);
		void FindNodes(const QStringList& filter, NodeItem* nodeItem, QVector<NodeItem*>& nodeItems);
		NodeItem* FindNode(const SceneNode* node, NodeItem* nodeItem);

		private slots: // handlers

		void UpdateMenuAndTable(QTreeWidgetItem* current, QTreeWidgetItem* previous);
		void UpdateField(QTableWidgetItem* item);
		void UpdateTextEdit(int currentRow, int currentColumn, int previousRow, int previousColumn);
		void EnableTextControls();
		void ApplyTextChanges();
		void RevertTextChanges();
		void FilterTree(QString text = QString());
		void ShowEditMenu(QPoint point);
	};
}}}

#endif // APPLICATION_GUI_DOCUMENTWINDOW_H
