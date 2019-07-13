#ifndef __IFLOWSYSTEM_H__
#define __IFLOWSYSTEM_H__

#pragma once

#include "ConfigurableVariant.h"
#include "SerializeFwd.h"

// Help message.
#define _HELP(x) x
#define _UICONFIG(x) x

typedef uint8 TFlowPortId;
typedef uint16 TFlowNodeId;
typedef uint16 TFlowNodeTypeId;

static const TFlowNodeId InvalidFlowNodeId = ~TFlowNodeId(0);
static const TFlowPortId InvalidFlowPortId = ~TFlowPortId(0);
static const TFlowNodeTypeId InvalidFlowNodeTypeId = 0; // must be 0! FlowSystem.cpp relies on it

// this is a special type which means "no input data"
struct SFlowSystemVoid 
{
	void Serialize(TSerialize ser) {}
};

struct FlowVec3 : public Vec3
{
	FlowVec3() : Vec3(0,0,0) {}
	FlowVec3( float x, float y, float z ) : Vec3(x,y,z) {}
};

// by adding types to this list, we can extend the flow system to handle
// new data types
// OTHER THINGS THAT NEED UPDATING HOWEVER INCLUDE:
//   CFlowData::ConfigureInputPort
typedef NTypelist::CConstruct<
	SFlowSystemVoid,
	int,
	float,
	EntityId,
	Vec3,
	string,
	bool
>::TType TFlowSystemDataTypes;

// default conversion uses C++ rules
template <class From, class To>
struct SFlowSystemConversion
{
	static ILINE bool ConvertValue( const From& from, To& to )
	{
		to = (To)from;
		return true;
	}
};

#define FLOWSYSTEM_NO_CONVERSION(T) \
	template <> struct SFlowSystemConversion<T,T> { \
	static ILINE bool ConvertValue( const T& from, T& to ) { to = from; return true; } \
	}
FLOWSYSTEM_NO_CONVERSION(SFlowSystemVoid);
FLOWSYSTEM_NO_CONVERSION(int);
FLOWSYSTEM_NO_CONVERSION(float);
FLOWSYSTEM_NO_CONVERSION(EntityId);
FLOWSYSTEM_NO_CONVERSION(Vec3);
FLOWSYSTEM_NO_CONVERSION(string);
FLOWSYSTEM_NO_CONVERSION(bool);
#undef FLOWSYSTEM_NO_CONVERSION

// specialization for converting to bool to avoid compiler warnings
template <class From>
struct SFlowSystemConversion<From, bool>
{
	static ILINE bool ConvertValue( const From& from, bool& to )
	{
		to = (from != 0);
		return true;
	}
};

// "void" conversion needs some help
// converting from void to anything yields default value of type
template <class To>
struct SFlowSystemConversion<SFlowSystemVoid, To>
{
	static ILINE bool ConvertValue( SFlowSystemVoid, To& to )
	{
		to = To();
		return true;
	}
};
template <>
struct SFlowSystemConversion<SFlowSystemVoid, bool>
{
	static ILINE bool ConvertValue( const SFlowSystemVoid& from, bool& to )
	{
		to = false;
		return true;
	}
};
template <>
struct SFlowSystemConversion<SFlowSystemVoid, Vec3>
{
	static ILINE bool ConvertValue( const SFlowSystemVoid& from, Vec3& to )
	{
		to = Vec3(0,0,0);
		return true;
	}
};
template <>
struct SFlowSystemConversion<Vec3, SFlowSystemVoid>
{
	static ILINE bool ConvertValue( const Vec3& from, SFlowSystemVoid& )
	{
		return true;
	}
};
// converting to void is trivial - just lose the value info
template <class From>
struct SFlowSystemConversion<From, SFlowSystemVoid>
{
	static ILINE bool ConvertValue( const From& from, SFlowSystemVoid& )
	{
		return true;
	}
};

// "Vec3" conversions...
template <class To>
struct SFlowSystemConversion<Vec3, To>
{
	static ILINE bool ConvertValue( const Vec3& from, To& to )
	{
		return SFlowSystemConversion<float, To>::ConvertValue( from.x, to );
	}
};
template <class From>
struct SFlowSystemConversion<From, Vec3>
{
	static ILINE bool ConvertValue( const From& from, Vec3& to )
	{
		float temp;
		if (!SFlowSystemConversion<From, float>::ConvertValue( from, temp ))
			return false;
		to.x = to.y = to.z = temp;
		return true;
	}
};
template <>
struct SFlowSystemConversion<Vec3, bool>
{
	static ILINE bool ConvertValue( const Vec3& from, bool& to )
	{
		to = from.GetLengthSquared() > 0;
		return true;
	}
};

#define FLOWSYSTEM_STRING_CONVERSION(type,fmt) \
	template <> \
	struct SFlowSystemConversion<type, string> \
	{ \
		static ILINE bool ConvertValue( const type& from, string& to ) \
		{ \
			to.Format( fmt, from ); \
			return true; \
		} \
	}; \
	template <> \
	struct SFlowSystemConversion<string, type> \
	{ \
		static ILINE bool ConvertValue( const string& from, type& to ) \
		{ \
			return 1 == sscanf( from.c_str(), fmt, &to ); \
		} \
	};

FLOWSYSTEM_STRING_CONVERSION(int, "%d");
FLOWSYSTEM_STRING_CONVERSION(float, "%f");
FLOWSYSTEM_STRING_CONVERSION(EntityId, "%u");
// FLOWSYSTEM_STRING_CONVERSION(bool, "%d");

#undef FLOWSYSTEM_STRING_CONVERSION

template <>
struct SFlowSystemConversion<bool, string>
{
	static ILINE bool ConvertValue( const bool& from, string& to )
	{
		to.Format( "%d", from );
		return true;
	}	
};

template <>
struct SFlowSystemConversion<string, bool>
{
	// leaves 'to' unchanged in case of error
	static ILINE bool ConvertValue( const string& from, bool& to )
	{
		int to_i;
		if (1 == sscanf( from.c_str(), "%d", &to_i ))
		{
			to = !!to_i;
			return true;
		}
		if (0 == stricmp (from.c_str(), "true"))
		{
			to = true;
			return true;
		}
		if (0 == stricmp (from.c_str(), "false"))
		{
			to = false;
			return true;
		}
		return false;
	}	
};

template <>
struct SFlowSystemConversion<Vec3, string>
{
	static ILINE bool ConvertValue( const Vec3& from, string& to )
	{
		to.Format( "%f,%f,%f", from.x, from.y, from.z );
		return true;
	}
};

template <>
struct SFlowSystemConversion<string, Vec3>
{
	static ILINE bool ConvertValue( const string& from, Vec3& to )
	{
		return 3 == sscanf( from.c_str(), "%f,%f,%f", &to.x, &to.y, &to.z );
	}
};

enum EFlowDataTypes
{
	eFDT_Any = -1,
	eFDT_Void = NTypelist::IndexOf<SFlowSystemVoid, TFlowSystemDataTypes>::value,
	eFDT_Int = NTypelist::IndexOf<int, TFlowSystemDataTypes>::value,
	eFDT_Float = NTypelist::IndexOf<float, TFlowSystemDataTypes>::value,
	eFDT_EntityId = NTypelist::IndexOf<EntityId, TFlowSystemDataTypes>::value,
	eFDT_Vec3 = NTypelist::IndexOf<Vec3, TFlowSystemDataTypes>::value,
	eFDT_String = NTypelist::IndexOf<string, TFlowSystemDataTypes>::value,
	eFDT_Bool = NTypelist::IndexOf<bool, TFlowSystemDataTypes>::value,
};

typedef CConfigurableVariant<TFlowSystemDataTypes, sizeof(void*), SFlowSystemConversion> TFlowInputData;

struct SEqualFlowInputData
{
	class CEqualVisitor
	{
	public:
		ILINE CEqualVisitor( const TFlowInputData& b ) : m_b(b) {}
		template <class T> ILINE void Visit( const T& value )
		{
			const T * ptrB = 0;
			m_b.GetPtr(&ptrB);
			assert( ptrB );
			m_result = value == *ptrB;
		}
		ILINE void Visit( const SFlowSystemVoid& value )
		{
			m_result = false;
		}
		ILINE bool result() { return m_result; }

	private:
		bool m_result;
		const TFlowInputData& m_b;
	};

	bool operator()( const TFlowInputData& a, const TFlowInputData& b ) const
	{
		int aTag = a.GetTag();
		int bTag = b.GetTag();
		if (aTag == bTag)
		{
			CEqualVisitor visitor(b);
			a.Visit( visitor );
			return visitor.result();
		}
		else // aTag != bTag
			return false;
	}
};

//////////////////////////////////////////////////////////////////////////
// TFlowInputData compare function.
inline bool IsEqualFlowInputData( const TFlowInputData &v1,const TFlowInputData &v2 )
{
	SEqualFlowInputData pred;
	return pred(v1,v2);
}

struct SFlowAddress
{
	SFlowAddress( TFlowNodeId node, TFlowPortId port, bool isOutput )
	{
		this->node = node;
		this->port = port;
		this->isOutput = isOutput;
	}
	SFlowAddress()
	{
		node = InvalidFlowNodeId;
		port = InvalidFlowPortId;
		isOutput = true;
	}
	TFlowNodeId node;
	TFlowPortId port;
	bool isOutput;
};

//////////////////////////////////////////////////////////////////////////
enum EFlowNodeFlags
{
	EFLN_TARGET_ENTITY         = 0x0001, // CORE FLAG: This node targets an entity, entity id must be provided.
	EFLN_HIDE_UI               = 0x0002, // CORE FLAG: This node cannot be selected by user for placement in flow graph UI.
	EFLN_CORE_MASK             = 0x000F, // CORE_MASK

	EFLN_APPROVED              = 0x0010, // CATEGORY:  This node is approved for designers
	EFLN_ADVANCED              = 0x0020, // CATEGORY:  This node is slightly advanced and approved
	EFLN_DEBUG                 = 0x0040, // CATEGORY:  This node is for debug purpose only
	EFLN_WIP                   = 0x0080, // CATEGORY:  This node is work-in-progress and shouldn't be used by designers
	EFLN_LEGACY                = 0x0100, // CATEGORY:  This node is legacy and will VERY soon vanish
	EFLN_NOCATEGORY            = 0x0800, // CATEGORY:  This node has no category yet! Default!
	EFLN_CATEGORY_MASK         = 0x0FF0, // CATEGORY_MASK

	EFLN_USAGE_MASK            = 0xF000, // USAGE_MASK
};

//////////////////////////////////////////////////////////////////////////
enum EFlowSpecialEntityId
{
	EFLOWNODE_ENTITY_ID_GRAPH1 = -1,
	EFLOWNODE_ENTITY_ID_GRAPH2 = -2,
};

struct SInputPortConfig
{
	// name of this port
	const char * name;
	// Human readable name of this port (default: same as name)
	const char * humanName;
	// Human readable description of this port (help)
	const char * description;
	// UIConfig: enums for the variable e.g 
	// "enum_string:a,b,c"
	// "enum_string:something=a,somethingelse=b,whatever=c"
	// "enum_int:something=0,somethingelse=10,whatever=20"
	// "enum_float:something=1.0,somethingelse=2.0"
	// "enum_global:GlobalEnumName"
	const char * sUIConfig;
	// default data
	TFlowInputData defaultData;
};

struct SOutputPortConfig
{
	// name of this port
	const char * name;
	// Human readable name of this port (default: same as name)
	const char * humanName;
	// Human readable description of this port (help)
	const char * description;
	// type of our output (or -1 for "dynamic")
	int type;
};

struct SFlowNodeConfig
{
	SFlowNodeConfig() : pInputPorts(0), pOutputPorts(0), nFlags(EFLN_NOCATEGORY), sDescription(0), sUIClassName(0) {}

	const SInputPortConfig * pInputPorts;
	const SOutputPortConfig * pOutputPorts;
	// Node configuration flags @see EFlowNodeFlags
	uint32 nFlags;
	const char *sDescription;
	const char *sUIClassName;

	ILINE void SetCategory(uint32 flags)
	{
		nFlags = (nFlags & ~EFLN_CATEGORY_MASK) | flags;
	}

	ILINE uint32 GetCategory() const
	{
		return /* static_cast<EFlowNodeFlags> */ (nFlags & EFLN_CATEGORY_MASK);
	}

	ILINE uint32 GetCoreFlags() const
	{
		return nFlags & EFLN_CORE_MASK;
	}

	ILINE uint32 GetUsageFlags() const
	{
		return nFlags & EFLN_USAGE_MASK;
	}

};

template <class T>
ILINE SOutputPortConfig OutputPortConfig( const char * name,const char *description=NULL, const char *humanName=NULL )
{
	SOutputPortConfig result = {name, humanName, description, NTypelist::IndexOf<T, TFlowSystemDataTypes>::value};
	return result;
}

ILINE SOutputPortConfig OutputPortConfig_AnyType( const char * name,const char *description=NULL, const char *humanName=NULL )
{
	SOutputPortConfig result = {name, humanName, description, eFDT_Any};
	return result;
}

ILINE SOutputPortConfig OutputPortConfig_Void( const char * name,const char *description=NULL, const char *humanName=NULL )
{
	SOutputPortConfig result = {name, humanName, description, eFDT_Void};
	return result;
}

template <class T>
ILINE SInputPortConfig InputPortConfig( const char * name,const char *description=NULL, const char *humanName=NULL, const char *sUIConfig=NULL )
{
	SInputPortConfig result = {name, humanName, description, sUIConfig, TFlowInputData(TFlowInputData::ConstructType<T>::Helper::ConstructedValue(), true)};
	return result;
}

template <class T>
ILINE SInputPortConfig InputPortConfig( const char * name, const T& value, const char *description=NULL, const char *humanName=NULL, const char *sUIConfig=NULL )
{
	SInputPortConfig result = {name, humanName, description, sUIConfig, TFlowInputData(value, true)};
	return result;
}

ILINE SInputPortConfig InputPortConfig_AnyType( const char * name, const char *description=NULL, const char *humanName=NULL, const char *sUIConfig=NULL  )
{
	SInputPortConfig result = {name, humanName, description, sUIConfig, TFlowInputData(0, false)};
	return result;
}

ILINE SInputPortConfig InputPortConfig_Void( const char * name, const char *description=NULL, const char *humanName=NULL, const char *sUIConfig=NULL )
{
	SInputPortConfig result = {name, humanName, description, sUIConfig, TFlowInputData(SFlowSystemVoid(),false)};
	return result;
}

namespace NFlowSystemUtils
{

	// this class helps define IFlowGraph by declaring typed virtual
	// functions corresponding to TFlowSystemDataTypes
	template <class TL>
	struct IFlowSystemTyped;

	template <class H>
	struct IFlowSystemTypedHelper
	{
		virtual void DoActivatePort( const SFlowAddress, const H& value ) = 0;
	};

	template <class H, class T>
	struct IFlowSystemTyped< NTypelist::CNode<H,T> > : public IFlowSystemTyped<T>, public IFlowSystemTypedHelper<H>
	{
	};

	template <>
	struct IFlowSystemTyped< NTypelist::CEnd >
	{
	};

}

struct IAIAction;
struct IFlowGraph;

struct IFlowNode;
TYPEDEF_AUTOPTR(IFlowNode);
typedef IFlowNode_AutoPtr IFlowNodePtr;

struct IFlowNode
{
	struct SActivationInfo
	{
		SActivationInfo( IFlowGraph	* pGraph = 0, TFlowNodeId myID = 0, void * pUserData = 0, TFlowInputData * pInputPorts = 0 )
		{
			this->pGraph = pGraph;
			this->myID = myID;
			this->m_pUserData = pUserData;
			this->pInputPorts = pInputPorts;
			this->pEntity = 0;
			this->connectPort = InvalidFlowPortId;
		}
		IFlowGraph * pGraph;
		TFlowNodeId myID;
		IEntity* pEntity;
		TFlowPortId connectPort;
		TFlowInputData * pInputPorts;
		void *m_pUserData;
	};

	enum EFlowEvent
	{
		eFE_Update,
		eFE_Activate,				 // Sent if one or more input ports have been activated
		eFE_FinalActivate,   // must be eFE_Activate+1 (same as activate, but at the end of FlowSystem:Update)
		eFE_Initialize,      // Sent once after level has been loaded. Is NOT called on Serialization!
		eFE_FinalInitialize, // must be eFE_Initialize+1
		eFE_SetEntityId,     // This event is send to set the entity of the FlowNode. Might also be sent in conjunction (pre) other events (like eFE_Initialize)
		eFE_Suspend,
		eFE_Resume,
		eFE_ConnectInputPort,
		eFE_DisconnectInputPort,
		eFE_ConnectOutputPort,
		eFE_DisconnectOutputPort,
		eFE_DontDoAnythingWithThisPlease
	};

	virtual void AddRef() = 0;
	virtual void Release() = 0;
	virtual IFlowNodePtr Clone( SActivationInfo * ) = 0;

	virtual void GetConfiguration( SFlowNodeConfig& ) = 0;
	virtual bool SerializeXML( SActivationInfo *, const XmlNodeRef& root, bool reading ) = 0;
	virtual void Serialize( SActivationInfo *, TSerialize ser ) = 0;
	virtual void ProcessEvent( EFlowEvent event, SActivationInfo * ) = 0;

	virtual void GetMemoryStatistics(ICrySizer * s) = 0;
};

// wraps IFlowNode for specific data
struct IFlowNodeData
{
	virtual IFlowNode * GetNode() = 0;
	virtual TFlowNodeTypeId GetNodeTypeId() = 0;
	virtual const char* GetNodeName() = 0;
	virtual void GetConfiguration( SFlowNodeConfig& ) = 0;
};

struct IFlowGraph;
TYPEDEF_AUTOPTR(IFlowGraph);
typedef IFlowGraph_AutoPtr IFlowGraphPtr;

struct IFlowGraphHook;
TYPEDEF_AUTOPTR(IFlowGraphHook);
typedef IFlowGraphHook_AutoPtr IFlowGraphHookPtr;

struct IFlowGraphHook
{
	virtual void AddRef() = 0;
	virtual void Release() = 0;
	virtual IFlowNodePtr CreateNode( IFlowNode::SActivationInfo*, TFlowNodeTypeId typeId ) = 0;
	// return false to disallow this creation!!
	virtual bool CreatedNode( TFlowNodeId id, const char * name, TFlowNodeTypeId typeId, IFlowNodePtr pNode ) = 0;
	// this gets called if CreatedNode was called, and then cancelled later
	virtual void CancelCreatedNode( TFlowNodeId id, const char * name, TFlowNodeTypeId typeId, IFlowNodePtr pNode ) = 0;
};

struct IFlowNodeIterator
{
	virtual void AddRef() = 0;
	virtual void Release() = 0;
	virtual IFlowNodeData* Next( TFlowNodeId &id ) = 0;
};

struct IFlowEdgeIterator
{
	struct Edge
	{
		TFlowNodeId fromNodeId;
		TFlowPortId fromPortId;
		TFlowNodeId toNodeId;
		TFlowPortId toPortId;
	};

	virtual void AddRef() = 0;
	virtual void Release() = 0;
	virtual bool Next( Edge& edge ) = 0;
};

TYPEDEF_AUTOPTR(IFlowNodeIterator);
typedef IFlowNodeIterator_AutoPtr IFlowNodeIteratorPtr;
TYPEDEF_AUTOPTR(IFlowEdgeIterator);
typedef IFlowEdgeIterator_AutoPtr IFlowEdgeIteratorPtr;

struct IFlowGraph : public NFlowSystemUtils::IFlowSystemTyped<TFlowSystemDataTypes>
{
	// refer to NFlowSystemUtils::IFlowSystemTyped to see extra per-flow system type
	// functions that are added to this interface

	virtual void AddRef() = 0;
	virtual void Release() = 0;
	virtual IFlowGraphPtr Clone() = 0;

	// Clears flow graph, deletes all nodes and edges.
	virtual void Clear() = 0;

	virtual void RegisterHook( IFlowGraphHookPtr ) = 0;
	virtual void UnregisterHook( IFlowGraphHookPtr ) = 0;

	virtual IFlowNodeIteratorPtr CreateNodeIterator() = 0;
	virtual IFlowEdgeIteratorPtr CreateEdgeIterator() = 0;

	// Assign id of the default entity associated with this flow graph.
	virtual void SetGraphEntity( EntityId id,int nIndex=0 ) = 0;
	// Retrieve id of the default entity associated with this flow graph.
	virtual EntityId GetGraphEntity( int nIndex ) = 0;

	// enable/disable the flowgraph - used from editor
	virtual void SetEnabled (bool bEnable) = 0;
	// check if flowgraph is currently enabled
	virtual bool IsEnabled() const = 0;

	// activate/deactivate the flowgraph - used from FlowGraphProxy during Runtime
	virtual void SetActive (bool bActive) = 0;
	// check if flowgraph is currently active
	virtual bool IsActive() const = 0;

	// primary game interface
	virtual void Update() = 0;
	virtual bool SerializeXML( const XmlNodeRef& root, bool reading ) = 0;
	virtual void Serialize( TSerialize ser ) = 0;
	// similar to Update, but sends an eFE_Initialize instead of an eFE_Update
	virtual void InitializeValues() = 0;

	// node manipulation
	virtual SFlowAddress ResolveAddress( const char * addr, bool isOutput ) = 0;
	virtual TFlowNodeId ResolveNode( const char * name ) = 0;
	virtual TFlowNodeId CreateNode( TFlowNodeTypeId typeId, const char *name, void *pUserData=0 ) = 0;
	virtual TFlowNodeId CreateNode( const char* typeName, const char *name, void *pUserData=0 ) = 0; // deprecated
	virtual IFlowNodeData* GetNodeData( TFlowNodeId id ) = 0;
	virtual void SetNodeName( TFlowNodeId id,const char *sName ) = 0;
	virtual const char* GetNodeName( TFlowNodeId id ) = 0;
	virtual TFlowNodeTypeId GetNodeTypeId( TFlowNodeId id ) = 0;
	virtual const char* GetNodeTypeName( TFlowNodeId id ) = 0;
	virtual void RemoveNode( const char * name ) = 0;
	virtual void RemoveNode( TFlowNodeId id ) = 0;
	virtual void SetUserData( TFlowNodeId id, const XmlNodeRef& data ) = 0;
	virtual XmlNodeRef GetUserData( TFlowNodeId id ) = 0;
	virtual bool LinkNodes( SFlowAddress from, SFlowAddress to ) = 0;
	virtual void UnlinkNodes( SFlowAddress from, SFlowAddress to ) = 0;

	virtual void SetEntityId( TFlowNodeId, EntityId ) = 0;
	virtual EntityId GetEntityId( TFlowNodeId ) = 0;

	// low level functions
	virtual void GetNodeConfiguration( TFlowNodeId id, SFlowNodeConfig& ) = 0;

	// node-graph interface
	virtual void SetRegularlyUpdated( TFlowNodeId, bool ) = 0;
	virtual void RequestFinalActivation( TFlowNodeId ) = 0;
	virtual void ActivateNode( TFlowNodeId ) = 0;
	virtual void ActivatePortAny( SFlowAddress output, const TFlowInputData& ) = 0;
	virtual bool SetInputValue( TFlowNodeId node, TFlowPortId port, const TFlowInputData& ) = 0;
	virtual bool IsOutputConnected( SFlowAddress output ) = 0;
	virtual const TFlowInputData* GetInputValue( TFlowNodeId node, TFlowPortId port ) = 0;

	template <class T>
	ILINE void ActivatePort( const SFlowAddress output, const T& value )
	{
		static_cast<NFlowSystemUtils::IFlowSystemTypedHelper<T>*>(this)->DoActivatePort( output, value );
	}
	ILINE void ActivatePort( SFlowAddress output, const TFlowInputData& value )
	{
		ActivatePortAny( output, value );
	}

	// temporary solutions [ ask Dejan ]

	// Suspended flow graphs were needed for AI Action flow graphs.
	// Suspended flow graphs shouldn't be updated!
	// Nodes in suspended flow graphs should ignore OnEntityEvent notifications!
	virtual void SetSuspended( bool suspend = true ) = 0;
	virtual bool IsSuspended() const = 0;

	// AI action related
	virtual void SetAIAction( IAIAction* pAIAction ) = 0;
	virtual IAIAction* GetAIAction() const = 0;

	virtual void GetMemoryStatistics(ICrySizer * s) = 0;
};

struct IFlowNodeFactory
{
	virtual void AddRef() = 0;
	virtual void Release() = 0;
	virtual IFlowNodePtr Create( IFlowNode::SActivationInfo* ) = 0;
	virtual void GetMemoryStatistics(ICrySizer * s) = 0;
};

TYPEDEF_AUTOPTR(IFlowNodeFactory);
typedef IFlowNodeFactory_AutoPtr IFlowNodeFactoryPtr;

struct IFlowNodeTypeIterator
{
	struct SNodeType
	{
		TFlowNodeTypeId typeId;
		const char*			typeName;
	};
	virtual void AddRef() = 0;
	virtual void Release() = 0;
	virtual bool Next(SNodeType& nodeType) = 0;
};
TYPEDEF_AUTOPTR(IFlowNodeTypeIterator);
typedef IFlowNodeTypeIterator_AutoPtr IFlowNodeTypeIteratorPtr;

struct IFlowGraphInspector
{
	struct IFilter;
	TYPEDEF_AUTOPTR(IFilter);
	typedef IFilter_AutoPtr IFilterPtr;

	struct IFilter
	{
		enum EFilterResult
		{
			eFR_DontCare, // let others decide [not used currently]
			eFR_Block,    // blocks flow record
			eFR_Pass      // lets flow record pass
		};
		virtual void AddRef() = 0;
		virtual void Release() = 0;
		// filter can have subfilters (these are generally ANDed)
		virtual void AddFilter(IFilterPtr pFilter) = 0;
		virtual void RemoveFilter(IFilterPtr pFilter) = 0;
		virtual IFlowGraphInspector::IFilter::EFilterResult Apply(IFlowGraph * pGraph, const SFlowAddress from, const SFlowAddress to) = 0;
	};


	virtual void AddRef() = 0;
	virtual void Release() = 0;
	virtual void PreUpdate(IFlowGraph * pGraph) = 0;   // called once per frame before IFlowSystem::Update
	virtual void PostUpdate(IFlowGraph * pGraph) = 0;  // called once per frame after  IFlowSystem::Update
	virtual void NotifyFlow(IFlowGraph * pGraph, const SFlowAddress from, const SFlowAddress to) = 0;
	virtual void NotifyProcessEvent(IFlowNode::EFlowEvent event, IFlowNode::SActivationInfo * pActInfo, IFlowNode * pImpl) = 0;
	virtual void AddFilter(IFlowGraphInspector::IFilterPtr pFilter) = 0;
	virtual void RemoveFilter(IFlowGraphInspector::IFilterPtr pFilter) = 0;

	virtual void GetMemoryStatistics(ICrySizer * s) = 0;
};

TYPEDEF_AUTOPTR(IFlowGraphInspector);
typedef IFlowGraphInspector_AutoPtr IFlowGraphInspectorPtr;

struct IFlowSystem
{
	virtual void Release() = 0;

	// IGameFramework calls this every frame
	virtual void Update() = 0;

	// Reset the flowsystem
	virtual void Reset() = 0;

	// Flowgraph creation
	virtual IFlowGraphPtr CreateFlowGraph() = 0;

	// type registration and resolving
	virtual TFlowNodeTypeId RegisterType( const char *typeName, IFlowNodeFactoryPtr factory ) = 0;
	virtual const char* GetTypeName( TFlowNodeTypeId typeId ) = 0;
	virtual TFlowNodeTypeId GetTypeId( const char* typeName ) = 0;
	virtual IFlowNodeTypeIteratorPtr CreateNodeTypeIterator() = 0;

	// inspecting flowsystem
	virtual void RegisterInspector( IFlowGraphInspectorPtr pInspector, IFlowGraphPtr pGraph = 0 ) = 0;
	virtual void UnregisterInspector( IFlowGraphInspectorPtr pInspector, IFlowGraphPtr pGraph = 0 ) = 0;
	virtual IFlowGraphInspectorPtr GetDefaultInspector() const = 0;
	virtual void EnableInspecting( bool bEnable ) = 0;
	virtual bool IsInspectingEnabled() const = 0;

	// memory
	virtual void GetMemoryStatistics(ICrySizer * s) = 0;
};

#endif
