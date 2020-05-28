#include "Utility/PreBase.h"

#include <QStringList>
#include <QTableWidgetItem>

#include "GUI/DocumentWindow.h"
#include "Application/Document.h"
#include "Scene/SceneNode.h"


using namespace Djbozkosz::Application::GUI;
using namespace Djbozkosz::Application::Scene;


DocumentWindow::DocumentWindow(Document* document, Definitions* definitions, QWidget* parent) :
	QWidget(parent),
	m_Ui(new Ui::DocumentWindow()),
	m_Document(document),
	m_Definitions(definitions)
{
	m_Ui->setupUi(this);

	connect(m_Ui->Tree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(UpdateTable(QTreeWidgetItem*,QTreeWidgetItem*)));
	connect(m_Ui->Table, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(UpdateField(QTableWidgetItem*)));
}

DocumentWindow::~DocumentWindow()
{
	disconnect(m_Ui->Tree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(UpdateTable(QTreeWidgetItem*,QTreeWidgetItem*)));
	disconnect(m_Ui->Table, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(UpdateField(QTableWidgetItem*)));

	delete m_Ui;
}

void DocumentWindow::SetupTree()
{
	auto tree = m_Ui->Tree;
	tree->clear();

	auto root     = m_Document->GetRoot();
	auto name     = GetNodeName(root);
	auto nodeItem = new NodeItem(tree, root, name);
	auto progress = 0.0f;
	CreateTree(nodeItem, root, progress);
	tree->addTopLevelItem(nodeItem);

	tree->expandItem(nodeItem);

	emit ProgressChanged(1.0f);
}

void DocumentWindow::CreateTree(NodeItem* nodeItem, Scene::SceneNode* node, float& progress)
{
	auto childs = node->Childs;
	foreach (child, childs)
	{
		auto childNode = *child;
		auto childName = GetNodeName(childNode);
		auto childItem = new NodeItem(nodeItem, childNode, childName);
		CreateTree(childItem, *child, progress);
		nodeItem->addChild(childItem);

		float progressInc;
		if (childNode->Definition == null || childNode->Definition->HasChilds == false)
		{
			progressInc = childNode->Size;
		}
		else
		{
			progressInc = sizeof(childNode->Type) + sizeof(childNode->Size);
		}
		progress += progressInc / m_Document->GetRoot()->Size;

		emit ProgressChanged(progress);
	}
}

void DocumentWindow::UpdateNode(NodeItem* nodeItem)
{
	nodeItem->setText(0, GetNodeName(nodeItem->Node));
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

QString DocumentWindow::GetNodeName(SceneNode* node) const
{
	if (node->Definition == null)
		return QString("Unknown node: 0x%1").arg(node->Type, 4, 16);

	auto nameInfos = m_Definitions->GetNodeNames(node->Type);
	if (nameInfos == null)
		return node->Definition->Name;

	QStringList names;

	foreach (nameInfo, *nameInfos)
	{
		auto childType    = nameInfo->ChildType;
		auto nodeWithName = (childType > 0) ? node->GetChild(childType) : node;

		if (nodeWithName == null)
			continue;

		auto fieldCtx   = SceneNodeUtility::FieldContext(nodeWithName, nameInfo->FieldIdx, &nodeWithName->Fields, &nodeWithName->Definition->Fields);
		auto nameFields = SceneNodeUtility::GetFieldDataAsString(fieldCtx);

		for (int idx = 0, count = nameFields.size(); idx < count; idx++)
		{
			auto enumValue = SceneNodeUtility::GetFieldDataEnum(m_Definitions, fieldCtx, idx);
			if (enumValue.isEmpty() == false)
			{
				nameFields[idx] += QString(" - %1").arg(enumValue);
			}
		}

		names.push_back(nameFields.join(", "));
	}

	return QString("%1 [%2]").arg(node->Definition->Name, names.join("; "));
}

void DocumentWindow::UpdateTable(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
	unused(previous);

	m_Ui->Table->blockSignals(true);
	SetupTable(as(current, NodeItem*));
	m_Ui->Table->blockSignals(false);
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
	UpdateTable(nodeItem, null);
	m_Ui->Table->setCurrentIndex(tabIndex);

	nodeItem = as(nodeItem->parent(), NodeItem*);
	if (nodeItem != null)
	{
		UpdateNode(nodeItem);
	}

	m_Document->SetDirty(true);
}
