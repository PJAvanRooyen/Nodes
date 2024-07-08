#include "Node.h"

namespace Shared{

template<class IdType>
INode<IdType>::INode()
{

}

template<class ConsumedDataType, class ProducedDataType>
ExecutableNode<ConsumedDataType, ProducedDataType>::ExecutableNode()
{

}

template<class ConsumedDataType, class ProducedDataType>
ProducedDataType
ExecutableNode<ConsumedDataType, ProducedDataType>::execute(ConsumedDataType data)
{

}


}
