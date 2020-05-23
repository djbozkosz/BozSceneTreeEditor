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

			explicit NodeItem(QTreeWidget* parent, SceneNode* node, QString& name);
			explicit NodeItem(NodeItem* parent, SceneNode* node, QString& name);
			virtual ~NodeItem();
		};

		sealed class FieldItem : public QTableWidgetItem
		{
			public:

			SceneNodeUtility::FieldContext FieldCtx;

			explicit inline FieldItem(const QString& text, const SceneNodeUtility::FieldContext& fieldCtx) : QTableWidgetItem(text), FieldCtx(fieldCtx) {}
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
		void CreateTree(NodeItem *item, SceneNode* node);

		void SetupTable(SceneNode* node);
		void SetupTableField(const SceneNodeUtility::FieldContext& fieldCtx, int& row);

		QString GetEnumValue(const SceneNodeUtility::FieldContext& fieldCtx, int data);
		QString GetNodeName(SceneNode* node) const;

		private slots: // handlers

		void UpdateTable(QTreeWidgetItem* current, QTreeWidgetItem* previous);
		void UpdateField(QTableWidgetItem* item);
	};
}}}

#endif // APPLICATION_GUI_DOCUMENTWINDOW_H
