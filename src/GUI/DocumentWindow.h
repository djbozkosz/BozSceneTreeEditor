#ifndef APPLICATION_GUI_DOCUMENTWINDOW_H
#define APPLICATION_GUI_DOCUMENTWINDOW_H

#include <QWidget>
#include <QTreeWidgetItem>

#include "ui_DocumentWindow.h"

#include "Scene/Definitions.h"


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

			explicit NodeItem(QTreeWidget* parent, Scene::SceneNode* node, QString& name);
			explicit NodeItem(NodeItem* parent, Scene::SceneNode* node, QString& name);
			virtual ~NodeItem();
		};

		sealed class FieldContext
		{
			public:

			Scene::SceneNode*                        Node;
			uint                                     FieldIdx;
			QVector<void*>*                          Fields;
			const Scene::Definitions::NodeFieldInfo* FieldInfo;

			explicit inline FieldContext(Scene::SceneNode* node, uint fieldIdx, QVector<void*>* fields, const Scene::Definitions::NodeFieldInfo* fieldInfo) :
				Node(node), FieldIdx(fieldIdx), Fields(fields), FieldInfo(fieldInfo)
			{
			}

			virtual inline ~FieldContext() {}
		};

		sealed class FieldItem : public QTableWidgetItem
		{
			public:

			FieldContext FieldCtx;

			explicit inline FieldItem(const QString& text, const FieldContext& fieldCtx) : QTableWidgetItem(text), FieldCtx(fieldCtx) {}
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
		Scene::SceneTree*   m_Tree;
		Scene::Definitions* m_Definitions;


		public: // methods

		explicit DocumentWindow(Document* document, Scene::Definitions* definitions, QWidget* parent = null);
		virtual ~DocumentWindow();

		inline Document* GetDocument() const { return m_Document; }

		private: // methods

		void SetupTree();
		void CreateTree(NodeItem *item, Scene::SceneNode* node);

		void SetupTable(Scene::SceneNode* node);
		void SetupTableField(ushort type, int& row, const FieldContext& fieldCtx);

		template <typename T> inline int SetTableFieldInt(int row, const FieldContext& fieldCtx, int count, int base = 10)
		{
			auto field = (*fieldCtx.Fields)[fieldCtx.FieldIdx];
			auto data  = reinterpret_cast<const T*>(field);
			for (int i = 0; i < count; i++)
			{
				auto text = QString::number(data[i], base);
				if (base == 16)
				{
					text = QString("0x%1").arg(text);
				}

				m_Ui->Table->setItem(row, i + 2, new FieldItem(text, fieldCtx));
			}

			return data[0];
		}

		inline void SetTableFieldFloat(int idx, const FieldContext& fieldCtx, int count)
		{
			auto field = (*fieldCtx.Fields)[fieldCtx.FieldIdx];
			auto data  = reinterpret_cast<const float*>(field);
			for (int i = 0; i < count; i++)
			{
				auto text = QString::number(data[i]);
				m_Ui->Table->setItem(idx, i + 2, new FieldItem(text, fieldCtx));
			}
		}

		template <typename T> inline void SetField(const T& value, void* field, uint offset = 0)
		{
			auto data = reinterpret_cast<T*>(field);
			data[offset] = value;
		}

		QString GetNodeName(Scene::SceneNode* node) const;

		private slots: // handlers

		void UpdateTable(QTreeWidgetItem* current, QTreeWidgetItem* previous);
		void UpdateField(QTableWidgetItem* item);
	};
}}}

#endif // APPLICATION_GUI_DOCUMENTWINDOW_H
