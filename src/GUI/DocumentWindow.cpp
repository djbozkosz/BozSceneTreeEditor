#include "Utility/PreBase.h"

#include <QStringList>
#include <QTableWidgetItem>

#include "GUI/DocumentWindow.h"
#include "Application/Document.h"
#include "Scene/SceneTree.h"
#include "Scene/SceneNode.h"


using namespace Djbozkosz::Application::GUI;
using namespace Djbozkosz::Application::Scene;


DocumentWindow::NodeItem::NodeItem(QTreeWidget* parent, Scene::SceneNode* node, QString& name) :
	QTreeWidgetItem(parent, QStringList() << name),
	Node(node)
{
}

DocumentWindow::NodeItem::NodeItem(NodeItem* parent, Scene::SceneNode* node, QString& name) :
	QTreeWidgetItem(parent, QStringList() << name),
	Node(node)
{
}

DocumentWindow::NodeItem::~NodeItem()
{
}


DocumentWindow::DocumentWindow(Document* document, Definitions* definitions, QWidget* parent) :
	QWidget(parent),
	m_Ui(new Ui::DocumentWindow()),
	m_Document(document),
	m_Tree(document->GetTree()),
	m_Definitions(definitions)
{
	m_Ui->setupUi(this);

	connect(m_Ui->Tree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(UpdateTable(QTreeWidgetItem*,QTreeWidgetItem*)));
	connect(m_Ui->Table, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(UpdateField(QTableWidgetItem*)));

	SetupTree();
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

	auto root = m_Tree->Root;
	auto name = GetNodeName(root);
	auto item = new NodeItem(tree, root, name);
	CreateTree(item, root);
	tree->addTopLevelItem(item);

	tree->expandItem(item);
}

void DocumentWindow::CreateTree(NodeItem* item, Scene::SceneNode* node)
{
	auto childs = node->Childs;
	foreach (child, childs)
	{
		auto childName = GetNodeName(*child);
		auto childItem = new NodeItem(item, *child, childName);
		CreateTree(childItem, *child);
		item->addChild(childItem);
	}
}

void DocumentWindow::SetupTable(SceneNode* node)
{
	auto table = m_Ui->Table;
	table->setRowCount(0);

	if (node == null)
		return;

	auto count = node->Fields.size();
	if (count == 0)
	{
		table->setRowCount(1);
		table->setItem(0, 0, new ReadOnlyItem("No data"));
		return;
	}

	if (node->Definition == null || node->Definition->Fields[0].FieldType->Type == Definitions::ENodeFieldType::Unknown)
	{
		table->setRowCount(1);
		table->setItem(0, 0, new ReadOnlyItem("Unknown data"));
		return;
	}

	auto row = 0;

	for (int fieldIdx = 0; fieldIdx < count; fieldIdx++, row++)
	{
		auto fieldInfo = &node->Definition->Fields[fieldIdx];

		table->setRowCount(table->rowCount() + 1);
		table->setItem(row, 0, new ReadOnlyItem(fieldInfo->Name));
		table->setItem(row, 1, new ReadOnlyItem(fieldInfo->FieldType->Name));
		SetupTableField(Scene::SceneNodeUtility::FieldContext(node, fieldIdx, &node->Fields, fieldInfo), row);
	}
}

void DocumentWindow::SetupTableField(const Scene::SceneNodeUtility::FieldContext &fieldCtx, int& row)
{
	auto        field     = (*fieldCtx.Fields)[fieldCtx.FieldIdx];
	const auto& fieldInfo = fieldCtx.FieldInfo;
	auto        fieldType = fieldInfo->FieldType->Type;

	switch (fieldType)
	{
		case Definitions::ENodeFieldType::Uint8:    SetTableFieldInt<uchar >(row, fieldCtx, 1    ); break;
		case Definitions::ENodeFieldType::Uint16:   SetTableFieldInt<ushort>(row, fieldCtx, 1    ); break;
		case Definitions::ENodeFieldType::Uint16_3: SetTableFieldInt<ushort>(row, fieldCtx, 3    ); break;
		case Definitions::ENodeFieldType::Uint32:   SetTableFieldInt<uint  >(row, fieldCtx, 1    ); break;
		case Definitions::ENodeFieldType::Int8:     SetTableFieldInt<char  >(row, fieldCtx, 1    ); break;
		case Definitions::ENodeFieldType::Int16:    SetTableFieldInt<short >(row, fieldCtx, 1    ); break;
		case Definitions::ENodeFieldType::Int32:    SetTableFieldInt<int   >(row, fieldCtx, 1    ); break;
		case Definitions::ENodeFieldType::Hex8:     SetTableFieldInt<uchar >(row, fieldCtx, 1, 16); break;
		case Definitions::ENodeFieldType::Hex16:    SetTableFieldInt<ushort>(row, fieldCtx, 1, 16); break;
		case Definitions::ENodeFieldType::Hex32:    SetTableFieldInt<uint  >(row, fieldCtx, 1, 16); break;
		case Definitions::ENodeFieldType::Float:    SetTableFieldFloat      (row, fieldCtx, 1    ); break;
		case Definitions::ENodeFieldType::Float2:   SetTableFieldFloat      (row, fieldCtx, 2    ); break;
		case Definitions::ENodeFieldType::Float3:   SetTableFieldFloat      (row, fieldCtx, 3    ); break;
		case Definitions::ENodeFieldType::Float4:   SetTableFieldFloat      (row, fieldCtx, 4    ); break;
		case Definitions::ENodeFieldType::Color:    SetTableFieldFloat      (row, fieldCtx, 3    ); break;

		case Definitions::ENodeFieldType::String:
		case Definitions::ENodeFieldType::StringFixed:
		{
			auto dataChar = reinterpret_cast<const char*>(field);
			m_Ui->Table->setItem(row, 2, new FieldItem(QString::fromLatin1(dataChar), fieldCtx));
			break;
		}

		case Definitions::ENodeFieldType::StringArray:
		case Definitions::ENodeFieldType::StringArray2:
		{
			auto dataChar      = reinterpret_cast<const char*>(field);
			auto dataUInt      = reinterpret_cast<const uint*>(field);
			auto sizeReduction = (fieldType == Definitions::ENodeFieldType::StringArray2) ? 1 : 0;
			m_Ui->Table->setItem(row, 2, new FieldItem(QString::fromLatin1(&dataChar[4], dataUInt[0] - sizeReduction), fieldCtx));
			break;
		}

		case Definitions::ENodeFieldType::Struct:
		{
			auto& structArray      = *reinterpret_cast<QVector<QVector<void*> >*>(field);
			auto  structArrayCount = structArray.size();

			auto*       table            = m_Ui->Table;
			table->setRowCount(table->rowCount() + fieldInfo->NestedField->Fields.size() * structArrayCount - 1);

			for (int structIdx = 0; structIdx < structArrayCount; structIdx++)
			{
				auto strukt = &structArray[structIdx];

				for (int fieldIdx = 0, count = strukt->size(); fieldIdx < count; fieldIdx++)
				{
					auto fieldInf = &fieldInfo->NestedField->Fields[fieldIdx];

					table->setItem(row, 0, new ReadOnlyItem(QString("%1: %2").arg(structIdx).arg(fieldInf->Name)));
					table->setItem(row, 1, new ReadOnlyItem(fieldInf->FieldType->Name));

					SetupTableField(SceneNodeUtility::FieldContext(fieldCtx.Node, fieldIdx, strukt, fieldInf), row);
					row++;
				}
			}

			row--;
			break;
		}

		default:
			break;
	}
}

QString DocumentWindow::GetNodeName(SceneNode* node) const
{
	if (node->Definition == null)
		return QString("Unknown node: 0x%1").arg(node->Type, 4, 16);

	auto nameInfo = m_Definitions->GetNodeName(node->Type);
	if (nameInfo != null)
	{
		auto childType = nameInfo->ChildType;
		auto nodeName  = (childType > 0) ? node->GetChild(childType) : node;

		QString name;

		if (nodeName != null)
		{
			name = QString::fromLatin1(reinterpret_cast<const char*>(nodeName->Fields[nameInfo->FieldIdx]));
		}

		return QString("%1 [%2]").arg(node->Definition->Name, name);
	}

	return node->Definition->Name;
}

void DocumentWindow::UpdateTable(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
	unused(previous);

	m_Ui->Table->blockSignals(true);
	SetupTable(as(current, NodeItem*)->Node);
	m_Ui->Table->blockSignals(false);
}

void DocumentWindow::UpdateField(QTableWidgetItem* item)
{
	auto fieldItem = as(item, FieldItem*);
	if (fieldItem == null)
		return;

	const auto& fieldCtx  = fieldItem->FieldCtx;
	auto        fieldType = fieldCtx.FieldInfo->FieldType->Type;
	auto        offset    = item->column() - 2;
	const auto& text      = item->text();

	switch (fieldType)
	{
		case Definitions::ENodeFieldType::Uint8:    SceneNodeUtility::SetFieldData<uchar >(fieldCtx, offset, text.toUShort()        ); break;
		case Definitions::ENodeFieldType::Uint16:   SceneNodeUtility::SetFieldData<ushort>(fieldCtx, offset, text.toUShort()        ); break;
		case Definitions::ENodeFieldType::Uint16_3: SceneNodeUtility::SetFieldData<ushort>(fieldCtx, offset, text.toUShort()        ); break;
		case Definitions::ENodeFieldType::Uint32:   SceneNodeUtility::SetFieldData<uint  >(fieldCtx, offset, text.toUInt()          ); break;
		case Definitions::ENodeFieldType::Int8:     SceneNodeUtility::SetFieldData<char  >(fieldCtx, offset, text.toShort()         ); break;
		case Definitions::ENodeFieldType::Int16:    SceneNodeUtility::SetFieldData<short >(fieldCtx, offset, text.toShort()         ); break;
		case Definitions::ENodeFieldType::Int32:    SceneNodeUtility::SetFieldData<int   >(fieldCtx, offset, text.toInt()           ); break;
		case Definitions::ENodeFieldType::Hex8:     SceneNodeUtility::SetFieldData<uchar >(fieldCtx, offset, text.toUShort(null, 16)); break;
		case Definitions::ENodeFieldType::Hex16:    SceneNodeUtility::SetFieldData<ushort>(fieldCtx, offset, text.toUShort(null, 16)); break;
		case Definitions::ENodeFieldType::Hex32:    SceneNodeUtility::SetFieldData<uint  >(fieldCtx, offset, text.toUInt  (null, 16)); break;
		case Definitions::ENodeFieldType::Float:    SceneNodeUtility::SetFieldData<float >(fieldCtx, offset, text.toFloat()         ); break;
		case Definitions::ENodeFieldType::Float2:   SceneNodeUtility::SetFieldData<float >(fieldCtx, offset, text.toFloat()         ); break;
		case Definitions::ENodeFieldType::Float3:   SceneNodeUtility::SetFieldData<float >(fieldCtx, offset, text.toFloat()         ); break;
		case Definitions::ENodeFieldType::Float4:   SceneNodeUtility::SetFieldData<float >(fieldCtx, offset, text.toFloat()         ); break;
		case Definitions::ENodeFieldType::Color:    SceneNodeUtility::SetFieldData<float >(fieldCtx, offset, text.toFloat()         ); break;

		default: break;
	}
}
