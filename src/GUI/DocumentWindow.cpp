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
	m_Definitions(definitions),
	m_FilterClearAction(null),
	m_FilterNodeItemBackup(null)
{
	m_Ui->setupUi(this);

	auto tree = m_Ui->Tree;
	tree->SetDocument(document);

	connect(tree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(UpdateMenuAndTable(QTreeWidgetItem*,QTreeWidgetItem*)));
	connect(tree, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(ShowEditMenu(QPoint)));

	auto table = m_Ui->Table;
	connect(table, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(UpdateField(QTableWidgetItem*)));
	connect(table, SIGNAL(currentCellChanged(int, int, int, int)), this, SLOT(UpdateTextEdit(int, int, int, int)));

	connect(m_Ui->TextEdit, SIGNAL(textChanged()), this, SLOT(EnableTextControls()));
	connect(m_Ui->Apply, SIGNAL(clicked()), this, SLOT(ApplyTextChanges()));
	connect(m_Ui->Revert, SIGNAL(clicked()), this, SLOT(RevertTextChanges()));
	connect(m_Ui->TreeSearch, SIGNAL(textEdited(QString)), this, SLOT(FilterTree(QString)));

	const auto& treeSearchChilds = m_Ui->TreeSearch->children();
	foreach (child, treeSearchChilds)
	{
		auto action = as(*child, QAction*);
		if (action != null)
		{
			m_FilterClearAction = action;
			connect(action, SIGNAL(triggered()), this, SLOT(FilterTree()));
			break;
		}
	}
}

DocumentWindow::~DocumentWindow()
{
	auto tree = m_Ui->Tree;
	disconnect(tree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(UpdateMenuAndTable(QTreeWidgetItem*,QTreeWidgetItem*)));
	disconnect(tree, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(ShowEditMenu(QPoint)));

	auto table = m_Ui->Table;
	disconnect(table, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(UpdateField(QTableWidgetItem*)));
	disconnect(table, SIGNAL(currentCellChanged(int, int, int, int)), this, SLOT(UpdateTextEdit(int, int, int, int)));

	disconnect(m_Ui->TextEdit, SIGNAL(textChanged()), this, SLOT(EnableTextControls()));
	disconnect(m_Ui->Apply, SIGNAL(clicked()), this, SLOT(ApplyTextChanges()));
	disconnect(m_Ui->Revert, SIGNAL(clicked()), this, SLOT(RevertTextChanges()));
	disconnect(m_Ui->TreeSearch, SIGNAL(textEdited(QString)), this, SLOT(FilterTree(QString)));
	disconnect(m_FilterClearAction, SIGNAL(triggered()), this, SLOT(FilterTree()));

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
	tree->setCurrentItem(nodeItem);

	emit ProgressChanged(1.0f);
}

bool DocumentWindow::AddNode(SceneNode* node, NodeItem* parentItem, int idx)
{
	auto tree     = m_Ui->Tree;
	auto root     = m_Document->GetRoot();
	auto name     = SceneNodeUtility::GetNodeName(node, m_Definitions);
	auto nodeItem = default_(NodeItem*);

	if (root == null)
	{
		Debug::Assert(m_Document->GetRoot() == null) << "There is currently set another node!";
		m_Document->SetRoot(node);

		nodeItem = new NodeItem(tree, node, name);
		tree->addTopLevelItem(nodeItem);
	}
	else
	{
		auto parentNode       = parentItem->Node;
		auto parentDefinition = parentNode->Definition;

		if (parentDefinition == null || parentDefinition->HasChilds == false)
		{
			Debug::Error() << "Child nodes are not allowed in node" << parentItem->text(0) << "!";
			return false;
		}

		parentNode->Childs.insert(idx, node);

		SceneNodeUtility::NodePath path;
		SceneNodeUtility::GetNodePath(path, root, parentNode);
		SceneNodeUtility::ApplyNodeSizeOffset(path, node->Size);

		nodeItem = new NodeItem(parentItem, node, name);
		//parentItem->insertChild(targetIdx, item); // this does not work !?!

		QVector<QTreeWidgetItem*> childs;
		childs.resize(parentItem->childCount());

		while (parentItem->childCount() > 0)
		{
			auto idx = parentItem->childCount() - 1;
			auto child = parentItem->child(idx);

			childs[idx] = child;
			parentItem->removeChild(child);
		}

		childs.insert(idx, nodeItem);
		parentItem->addChildren(childs.toList());
	}

	auto progress = 1.0f;
	CreateTree(nodeItem, node, progress);

	tree->setCurrentItem(nodeItem);
	UpdateMenuAndTable(nodeItem, null);

	m_Document->SetDirty();
	return true;
}

void DocumentWindow::RemoveNode(NodeItem* nodeItem)
{
	auto tree       = m_Ui->Tree;
	auto parentItem = nodeItem->parent();
	auto node       = nodeItem->Node;

	if (parentItem == null)
	{
		m_Document->SetRoot(null);
		tree->takeTopLevelItem(0);
	}
	else
	{
		auto idx = parentItem->indexOfChild(nodeItem);
		parentItem->removeChild(nodeItem);

		SceneNodeUtility::NodePath path;
		SceneNodeUtility::GetNodePath(path, m_Document->GetRoot(), node);
		path.pop_front();

		auto parentNode = path.front();
		parentNode->Childs.removeAt(idx);
		SceneNodeUtility::ApplyNodeSizeOffset(path, -node->Size);
	}

	delete nodeItem;
	delete node;

	UpdateMenuAndTable(tree->currentItem(), null);
	m_Document->SetDirty();
}

NodeItem* DocumentWindow::GetSelectedNode() const
{
	auto nodeItem = m_Ui->Tree->currentItem();
	return as(nodeItem, NodeItem*);
}

SceneNode* DocumentWindow::GetSelectedSceneNode() const
{
	auto nodeItem = GetSelectedNode();
	if (nodeItem == null)
		return null;

	return nodeItem->Node;
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
	auto table = m_Ui->Table;
	table->setRowCount(0);

	if (nodeItem == null)
		return;

	auto node  = nodeItem->Node;
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

	m_Ui->TextEdit->setText(QString());
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

void DocumentWindow::SwitchEditMode(bool textChanged)
{
	auto textUnchanged = (textChanged == false);

	m_Ui->TextFrameControls->setEnabled(textChanged);
	m_Ui->Table->            setEnabled(textUnchanged);
	m_Ui->TreeFrame->        setEnabled(textUnchanged);

	EditMenuUpdateRequested(textUnchanged ? as(m_Ui->Tree->currentItem(), NodeItem*) : null, textChanged);
}

void DocumentWindow::FindNodes(const QStringList& filter, NodeItem* nodeItem, QVector<NodeItem*>& nodeItems)
{
	const auto& name = nodeItem->text(0);
	foreach (token, filter)
	{
		if (name.toLower().contains(*token) == true)
		{
			nodeItems.push_back(nodeItem);
		}
	}

	for (int idx = 0, count = nodeItem->childCount(); idx < count; idx++)
	{
		auto child = as(nodeItem->child(idx), NodeItem*);
		FindNodes(filter, child, nodeItems);
	}
}

NodeItem* DocumentWindow::FindNode(const SceneNode* node, NodeItem* nodeItem)
{
	if (nodeItem->Node == node)
		return nodeItem;

	for (int idx = 0, count = nodeItem->childCount(); idx < count; idx++)
	{
		auto child     = as(nodeItem->child(idx), NodeItem*);
		auto childNode = FindNode(node, child);

		if (childNode != null)
			return childNode;
	}

	return null;
}

void DocumentWindow::UpdateMenuAndTable(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
	unused(previous);

	auto nodeItem = as(current, NodeItem*);

	emit EditMenuUpdateRequested(nodeItem);

	auto table = m_Ui->Table;
	table->blockSignals(true);
	SetupTable(nodeItem);
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

void DocumentWindow::UpdateTextEdit(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
	unused(currentColumn);
	unused(previousRow);
	unused(previousColumn);

	auto table = m_Ui->Table;
	auto item  = as(table->item(currentRow, 2), FieldItem*);
	auto text  = (item != null) ? item->text() : QString();

	m_Ui->TextEdit->setText(text);
}

void DocumentWindow::EnableTextControls()
{
	if (m_Ui->TextEdit->hasFocus() == false)
		return;

	SwitchEditMode(true);
}

void DocumentWindow::ApplyTextChanges()
{
	auto table = m_Ui->Table;
	auto item  = as(table->item(table->currentRow(), 2), FieldItem*);
	item->setText(m_Ui->TextEdit->toPlainText());

	SwitchEditMode(false);
}

void DocumentWindow::RevertTextChanges()
{
	auto table = m_Ui->Table;
	auto item  = as(table->item(table->currentRow(), 2), FieldItem*);
	m_Ui->TextEdit->setText(item->text());

	SwitchEditMode(false);
}

void DocumentWindow::FilterTree(QString text)
{
	auto tree = m_Ui->Tree;

	if (m_FilterNodeItemBackup == null)
	{
		m_FilterNodeItemBackup = as(tree->topLevelItem(0), NodeItem*);
		tree->takeTopLevelItem(0);
	}
	else
	{
		auto selectedNode = as(tree->currentItem(), NodeItem*);

		while (tree->topLevelItemCount() > 0)
		{
			delete tree->takeTopLevelItem(0);
		}

		if (text.isEmpty() == true)
		{
			tree->addTopLevelItem(m_FilterNodeItemBackup);

			if (selectedNode != null)
			{
				auto filteredSelectedNode = FindNode(selectedNode->Node, m_FilterNodeItemBackup);
				tree->setCurrentItem(filteredSelectedNode);
				tree->expandItem(filteredSelectedNode);
			}

			m_FilterNodeItemBackup = null;
			return;
		}
	}

	QVector<NodeItem*> nodeItems;
	auto filter = text.toLower().split(' ', QString::SkipEmptyParts);

	FindNodes(filter, m_FilterNodeItemBackup, nodeItems);

	foreach (item, nodeItems)
	{
		auto nodeItem     = *item;
		auto filteredNode = new NodeItem(tree, nodeItem->Node, nodeItem->text(0));
		auto progress     = 1.0f;

		tree->addTopLevelItem(filteredNode);
		CreateTree(filteredNode, filteredNode->Node, progress);
	}
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
