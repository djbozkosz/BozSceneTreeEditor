#include "Utility/PreBase.h"

#include <QStringList>
#include <QTableWidgetItem>
#include <QMenu>
#include <QPoint>
#include <QEvent>
#include <QDragEnterEvent>
#include <QDropEvent>

#include "ui_DocumentWindow.h"

#include "GUI/DocumentWindow.h"
#include "Application/Document.h"
#include "Scene/SceneNode.h"


using namespace Djbozkosz::Application::GUI;
using namespace Djbozkosz::Application::Scene;


TreeWidget::TreeWidget(QWidget* parent) :
	QTreeWidget(parent),
	m_Document(null),
	m_DraggedNode(null)
{
}

TreeWidget::~TreeWidget()
{
}

void TreeWidget::dragEnterEvent(QDragEnterEvent* event)
{
	auto item = currentItem();
	if (item == topLevelItem(0))
	{
		event->ignore();
		return;
	}

	m_DraggedNode = as(item, NodeItem*);

	QTreeWidget::dragEnterEvent(event);
}

void TreeWidget::dropEvent(QDropEvent* event)
{
	auto oldParent = m_DraggedNode->parent();
	auto oldIdx    = oldParent->indexOfChild(m_DraggedNode);

	QTreeWidget::dropEvent(event);
	auto newParent = as(m_DraggedNode->parent(), NodeItem*);

	if (newParent == null)
	{
		auto newIdx = indexOfTopLevelItem(m_DraggedNode);
		takeTopLevelItem(newIdx);
		oldParent->insertChild(oldIdx, m_DraggedNode);

		Debug::Error() << "Child nodes are not allowed to be in root!";
	}
	else
	{
		auto newIdx = newParent->indexOfChild(m_DraggedNode);
		auto result = SceneNodeUtility::MoveNode(m_DraggedNode->Node, m_Document->GetRoot(), as(oldParent, NodeItem*)->Node, newParent->Node, oldIdx, newIdx);

		if (result == true)
		{
			m_Document->SetDirty();
		}
		else
		{
			newParent->removeChild(m_DraggedNode);
			oldParent->insertChild(oldIdx, m_DraggedNode);

			Debug::Error() << "Child nodes are not allowed in node" << newParent->text(0) << "!";
		}
	}

	m_DraggedNode = null;
}


DocumentWindow::DocumentWindow(Document* document, Definitions* definitions, QWidget* parent) :
	QWidget(parent),
	m_Ui(new Ui::DocumentWindow()),
	m_Document(document),
	m_Definitions(definitions)
{
	m_Ui->setupUi(this);

	auto tree = m_Ui->Tree;
	tree->SetDocument(document);

	connect(tree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(UpdateMenuAndTable(QTreeWidgetItem*,QTreeWidgetItem*)));
	connect(tree, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(ShowEditMenu(QPoint)));

	connect(m_Ui->Table, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(UpdateField(QTableWidgetItem*)));
}

DocumentWindow::~DocumentWindow()
{
	auto tree = m_Ui->Tree;
	disconnect(tree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(UpdateMenuAndTable(QTreeWidgetItem*,QTreeWidgetItem*)));
	disconnect(tree, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(ShowEditMenu(QPoint)));

	disconnect(m_Ui->Table, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(UpdateField(QTableWidgetItem*)));

	delete m_Ui;
}

void DocumentWindow::SetupTree()
{
	auto tree = m_Ui->Tree;
	tree->blockSignals(true);
	tree->clear();

	auto root = m_Document->GetRoot();
	if (root == null)
		return;

	auto name     = SceneNodeUtility::GetNodeName(root, m_Definitions);
	auto nodeItem = new NodeItem(tree, root, name);
	auto progress = 0.0f;
	CreateTree(nodeItem, root, progress);
	tree->addTopLevelItem(nodeItem);

	tree->blockSignals(false);
	tree->expandItem(nodeItem);
	tree->setFocus();

	nodeItem->setSelected(true);

	emit ProgressChanged(1.0f);
}

void DocumentWindow::AddNode(SceneNode* node)
{
	auto root    = m_Document->GetRoot();
	auto sibling = GetSelectedNode();
	auto name    = SceneNodeUtility::GetNodeName(node, m_Definitions);
	auto item    = default_(NodeItem*);

	if (root == null)
	{
		m_Document->SetRoot(node);

		auto tree = m_Ui->Tree;
		item = new NodeItem(tree, node, name);
		tree->addTopLevelItem(item);
	}
	else
	{
		SceneNodeUtility::NodePath path;
		SceneNodeUtility::GetNodePath(path, root, sibling->Node);
		path.pop_front();

		auto parentNode = path.back();
		auto parentItem = as(sibling->parent(), NodeItem*);
		auto siblingIdx = parentItem->indexOfChild(sibling);
		auto targetIdx  = siblingIdx + 1;

		parentNode->Childs.insert(targetIdx, node);
		SceneNodeUtility::ApplyNodeSizeOffset(path, node->Size);

		item = new NodeItem(parentItem, node, name);
		//parentItem->insertChild(targetIdx, item); // this does not work !?

		QVector<QTreeWidgetItem*> childs;
		childs.resize(parentItem->childCount());

		while (parentItem->childCount() > 0)
		{
			auto idx = parentItem->childCount() - 1;
			auto child = parentItem->child(idx);

			childs[idx] = child;
			parentItem->removeChild(child);
		}

		childs.insert(targetIdx, item);
		parentItem->addChildren(childs.toList());
	}

	if (sibling != null)
	{
		sibling->setSelected(false);
	}

	item->setSelected(true);
	UpdateMenuAndTable(item, sibling);

	m_Document->SetDirty();
}

NodeItem* DocumentWindow::GetSelectedNode() const
{
	auto item = m_Ui->Tree->currentItem();
	return as(item, NodeItem*);
}

SceneNode* DocumentWindow::GetSelectedSceneNode() const
{
	auto item = GetSelectedNode();
	if (item == null)
		return null;

	return item->Node;
}

void DocumentWindow::CreateTree(NodeItem* nodeItem, Scene::SceneNode* node, float& progress)
{
	auto childs = node->Childs;
	foreach (child, childs)
	{
		auto childNode = *child;
		auto childName = SceneNodeUtility::GetNodeName(childNode, m_Definitions);
		auto childItem = new NodeItem(nodeItem, childNode, childName);
		CreateTree(childItem, *child, progress);
		nodeItem->addChild(childItem);

		UpdateProgress(childNode, progress);
	}
}

void DocumentWindow::UpdateNode(NodeItem* nodeItem)
{
	nodeItem->setText(0, SceneNodeUtility::GetNodeName(nodeItem->Node, m_Definitions));
}

void DocumentWindow::SetupTable(NodeItem* nodeItem)
{
	auto node  = nodeItem->Node;
	auto table = m_Ui->Table;
	table->setRowCount(0);

	if (node == null)
		return;

	auto count = node->Fields.size();

	if (count == 0 || node->Definition == null || node->Definition->Fields[0].FieldType->Type == Definitions::ENodeFieldType::Unknown)
	{
		table->setRowCount(1);
		table->setItem(0, 0, new ReadOnlyItem((count == 0) ? "No data" : "Unknown data"));

		for (int idx = 1; idx < 6; idx++)
		{
			table->setItem(0, idx, new ReadOnlyItem(QString()));
		}

		return;
	}

	auto row = 0;

	for (int fieldIdx = 0; fieldIdx < count; fieldIdx++, row++)
	{
		auto& fieldInfos = node->Definition->Fields;
		auto& fieldInfo  = fieldInfos[fieldIdx];

		table->setRowCount(table->rowCount() + 1);
		table->setItem(row, 0, new ReadOnlyItem(fieldInfo.Name));
		table->setItem(row, 1, new ReadOnlyItem(fieldInfo.FieldType->Name));
		SetupTableField(nodeItem, Scene::SceneNodeUtility::FieldContext(node, fieldIdx, &node->Fields, &fieldInfos), row);
	}
}

void DocumentWindow::SetupTableField(NodeItem* nodeItem, const Scene::SceneNodeUtility::FieldContext &fieldCtx, int& row)
{
	auto table     = m_Ui->Table;
	auto field     = fieldCtx.GetField();
	auto fieldInfo = fieldCtx.GetFieldInfo();
	auto fieldType = fieldInfo->FieldType->Type;

	if (fieldType > Definitions::ENodeFieldType::Unknown && fieldType < Definitions::ENodeFieldType::Struct)
	{
		auto data      = SceneNodeUtility::GetFieldDataAsString(fieldCtx);
		auto dataCount = data.count();

		for (int idx = 0; idx < 4; idx++)
		{
			if (idx < dataCount)
			{
				auto text      = data[idx];
				auto enumValue = SceneNodeUtility::GetFieldDataEnum(m_Definitions, fieldCtx, idx);

				if (enumValue.isEmpty() == false)
				{
					text = QString("%1 - %2").arg(text).arg(enumValue);
				}

				table->setItem(row, 2 + idx, new FieldItem(text, nodeItem, fieldCtx));
			}
			else
			{
				table->setItem(row, 2 + idx, new ReadOnlyItem(QString()));
			}
		}
	}
	else if (fieldType == Definitions::ENodeFieldType::Struct)
	{
		auto& structArray      = *reinterpret_cast<Definitions::StructField*>(field);
		auto  structArrayCount = structArray.size();

		table->setRowCount(table->rowCount() + fieldInfo->NestedField->Fields.size() * structArrayCount - 1);

		for (int structIdx = 0; structIdx < structArrayCount; structIdx++)
		{
			auto strukt = &structArray[structIdx];

			for (int fieldIdx = 0, count = strukt->size(); fieldIdx < count; fieldIdx++)
			{
				auto& fieldInfs = fieldInfo->NestedField->Fields;
				auto& fieldInf  = fieldInfs[fieldIdx];

				table->setItem(row, 0, new ReadOnlyItem(QString("%1: %2").arg(structIdx).arg(fieldInf.Name)));
				table->setItem(row, 1, new ReadOnlyItem(fieldInf.FieldType->Name));

				SetupTableField(nodeItem, SceneNodeUtility::FieldContext(fieldCtx.Node, fieldIdx, strukt, &fieldInfs), row);
				row++;
			}
		}

		row--;
	}
}

void DocumentWindow::UpdateProgress(SceneNode* node, float& progress)
{
	float progressInc;
	if (node->Definition == null || node->Definition->HasChilds == false)
	{
		progressInc = node->Size;
	}
	else
	{
		progressInc = sizeof(node->Type) + sizeof(node->Size);
	}
	progress += progressInc / m_Document->GetRoot()->Size;

	emit ProgressChanged(progress);
}

void DocumentWindow::UpdateMenuAndTable(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
	unused(previous);

	emit EditMenuUpdateRequested();

	auto table = m_Ui->Table;
	table->blockSignals(true);
	SetupTable(as(current, NodeItem*));
	table->blockSignals(false);
}

void DocumentWindow::UpdateField(QTableWidgetItem* item)
{
	auto fieldItem = as(item, FieldItem*);
	if (fieldItem == null)
		return;

	SceneNodeUtility::SetFieldDataFromString(m_Document->GetRoot(), fieldItem->FieldCtx, item->text(), item->column() - 2);

	auto nodeItem = fieldItem->Item;
	UpdateNode(nodeItem);

	auto tabIndex = m_Ui->Table->currentIndex();
	UpdateMenuAndTable(nodeItem, null);
	m_Ui->Table->setCurrentIndex(tabIndex);

	nodeItem = as(nodeItem->parent(), NodeItem*);
	if (nodeItem != null)
	{
		UpdateNode(nodeItem);
	}

	m_Document->SetDirty();
}

void DocumentWindow::ShowEditMenu(QPoint point)
{
	auto tree     = m_Ui->Tree;
	auto nodeItem = as(tree->itemAt(point), NodeItem*);

	if (nodeItem == null)
		return;

	auto globalPoint = tree->mapToGlobal(point);
	emit EditMenuShowRequested(globalPoint);
}
