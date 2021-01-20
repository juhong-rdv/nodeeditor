#include "DataFlowGraphicsScene.hpp"

#include <stdexcept>
#include <utility>

#include <QtWidgets/QGraphicsSceneMoveEvent>
#include <QtWidgets/QFileDialog>

#include <QtCore/QBuffer>
#include <QtCore/QByteArray>
#include <QtCore/QDataStream>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QtGlobal>

#include "ConnectionGraphicsObject.hpp"
#include "DataModelRegistry.hpp"
#include "GraphicsView.hpp"
#include "NodeGraphicsObject.hpp"

namespace QtNodes
{


DataFlowGraphicsScene::
DataFlowGraphicsScene(DataFlowGraphModel & graphModel,
                      std::shared_ptr<DataModelRegistry> modelRegistry,
                      QObject * parent)
  : BasicGraphicsScene(graphModel, parent)
  , _graphModel(graphModel)
  , _modelRegistry(modelRegistry)
{
  setItemIndexMethod(QGraphicsScene::NoIndex);

}


// TODO constructor for an empyt scene?
// TODO constructor for an empyt scene?
// TODO constructor for an empyt scene?
// TODO constructor for an empyt scene?


DataFlowGraphicsScene::
~DataFlowGraphicsScene()
{}


DataModelRegistry &
DataFlowGraphicsScene::
registry() const
{
  return *_modelRegistry;
}


void
DataFlowGraphicsScene::
setRegistry(std::shared_ptr<DataModelRegistry> registry)
{
  _modelRegistry = registry;
}


//------------------------------------------------------------------------------

std::vector<NodeId>
DataFlowGraphicsScene::
selectedNodes() const
{
  QList<QGraphicsItem*> graphicsItems = selectedItems();

  std::vector<NodeId> result;
  result.reserve(graphicsItems.size());

  for (QGraphicsItem* item : graphicsItems)
  {
    auto ngo = qgraphicsitem_cast<NodeGraphicsObject*>(item);

    if (ngo != nullptr)
    {
      result.push_back(ngo->nodeId());
    }
  }

  return result;
}


#if 0
std::shared_ptr<Connection>
DataFlowGraphicsScene::
restoreConnection(QJsonObject const &connectionJson)
{
  QUuid nodeInId  = QUuid(connectionJson["in_id"].toString());
  QUuid nodeOutId = QUuid(connectionJson["out_id"].toString());

  PortIndex portIndexIn  = connectionJson["in_index"].toInt();
  PortIndex portIndexOut = connectionJson["out_index"].toInt();

  auto nodeIn  = _nodes[nodeInId].get();
  auto nodeOut = _nodes[nodeOutId].get();

  auto getConverter = [&]()
                      {
                        QJsonValue converterVal = connectionJson["converter"];

                        if (!converterVal.isUndefined())
                        {
                          QJsonObject converterJson = converterVal.toObject();

                          NodeDataType inType { converterJson["in"].toObject()["id"].toString(),
                                                converterJson["in"].toObject()["name"].toString() };

                          NodeDataType outType { converterJson["out"].toObject()["id"].toString(),
                                                 converterJson["out"].toObject()["name"].toString() };

                          auto converter  =
                            registry().getTypeConverter(outType, inType);

                          if (converter)
                            return converter;
                        }

                        return TypeConverter{};
                      };

  std::shared_ptr<Connection> connection =
    createConnection(*nodeIn, portIndexIn,
                     *nodeOut, portIndexOut,
                     getConverter());

  // Note: the connectionCreated(...) signal has already been sent
  // by createConnection(...)

  return connection;
}


Node&
DataFlowGraphicsScene::
restoreNode(QJsonObject const& nodeJson)
{
  QString modelName = nodeJson["model"].toObject()["name"].toString();

  auto dataModel = registry().create(modelName);

  if (!dataModel)
    throw std::logic_error(std::string("No registered model with name ") +
                           modelName.toLocal8Bit().data());

  auto node = detail::make_unique<Node>(std::move(dataModel));
  auto ngo  = detail::make_unique<NodeGraphicsObject>(*this, *node);
  node->setGraphicsObject(std::move(ngo));

  node->restore(nodeJson);

  auto nodePtr = node.get();
  _nodes[node->id()] = std::move(node);

  nodePlaced(*nodePtr);
  nodeCreated(*nodePtr);
  return *nodePtr;
}


#endif

void
DataFlowGraphicsScene::
save() const
{
  //QString fileName =
    //QFileDialog::getSaveFileName(nullptr,
                                 //tr("Open Flow Scene"),
                                 //QDir::homePath(),
                                 //tr("Flow Scene Files (*.flow)"));

  //if (!fileName.isEmpty())
  //{
    //if (!fileName.endsWith("flow", Qt::CaseInsensitive))
      //fileName += ".flow";

    //QFile file(fileName);
    //if (file.open(QIODevice::WriteOnly))
    //{
      //file.write(saveToMemory());
    //}
  //}
}

void
DataFlowGraphicsScene::
load()
{
  //QString fileName =
    //QFileDialog::getOpenFileName(nullptr,
                                 //tr("Open Flow Scene"),
                                 //QDir::homePath(),
                                 //tr("Flow Scene Files (*.flow)"));

  //if (!QFileInfo::exists(fileName))
    //return;

  //QFile file(fileName);

  //if (!file.open(QIODevice::ReadOnly))
    //return;

  //clearScene();

  //QByteArray wholeFile = file.readAll();

  //loadFromMemory(wholeFile);
}

#if 0


QByteArray
DataFlowGraphicsScene::
saveToMemory() const
{
  QJsonObject sceneJson;

  QJsonArray nodesJsonArray;

  for (auto const & pair : _nodes)
  {
    auto const &node = pair.second;

    nodesJsonArray.append(node->save());
  }

  sceneJson["nodes"] = nodesJsonArray;

  QJsonArray connectionJsonArray;
  for (auto const & pair : _connections)
  {
    auto const &connection = pair.second;

    QJsonObject connectionJson = connection->save();

    if (!connectionJson.isEmpty())
      connectionJsonArray.append(connectionJson);
  }

  sceneJson["connections"] = connectionJsonArray;

  QJsonDocument document(sceneJson);

  return document.toJson();
}


void
DataFlowGraphicsScene::
loadFromMemory(const QByteArray& data)
{
  QJsonObject const jsonDocument = QJsonDocument::fromJson(data).object();

  QJsonArray nodesJsonArray = jsonDocument["nodes"].toArray();

  for (QJsonValueRef node : nodesJsonArray)
  {
    restoreNode(node.toObject());
  }

  QJsonArray connectionJsonArray = jsonDocument["connections"].toArray();

  for (QJsonValueRef connection : connectionJsonArray)
  {
    restoreConnection(connection.toObject());
  }
}


#endif


}
