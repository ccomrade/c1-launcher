/********************************************************************
	Crytek Source File.
  Copyright (C), Crytek Studios, 2001-2004.
 -------------------------------------------------------------------------
  File name:   AITrackPatternDescriptor.h
	$Id$
  Description: 
  
 -------------------------------------------------------------------------
  History:
  - 1/9/2005   : Created by Mikko Mononen

*********************************************************************/


#ifndef __AITrackPatternDescriptor_H__
#define __AITrackPatternDescriptor_H__

#pragma once


/// The method to choose the next node when certain node is reached. Should be one of the following:
enum ETrackPatternBranchMethod
{
	/// Chooses on node from the list in linear sequence favoring the most or least exposed point (see ITrackPatternDescriptor::SetExposureMod).
	AITRACKPAT_CHOOSE_ALWAYS,

	/// Chooses the least deformed point in the list. Each node is assoaciated with a deformation value
	/// (percentage) which describes how much it was required to move in order to stay within the physical world.
	/// These deformation values are summed down to the parent nodes so that deformation at the end of
	/// the hierarchy will be caught down the hierarchy.
	AITRACKPAT_CHOOSE_LESS_DEFORMED,

	/// Chooses one point in the list randomly.
	AITRACKPAT_CHOOSE_RANDOM,

	/// Chooses the most exposed node, the current node and all the links included (see ITrackPatternDescriptor::SetExposureMod).
	AITRACKPAT_CHOOSE_MOST_EXPOSED,
};


/// Defines the node evaluation flags, the flags are as follows and can be combined:
enum ETrackPatternNodeFlags
{
	/// If this flag is set, this node can be used as the first node in the pattern.
	/// There can be multiple start nodes. In that case the closest one is chosen.
	AITRACKPAT_NODE_START						= 0x01,

	/// If this flag is set, the offset is interpret as an offset from the pattern center,
	/// otherwise the offset is offset from the start position.
	AITRACKPAT_NODE_ABSOLUTE				= 0x02,

	/// If this flag is set, a signal "OnReachedTrackPatternNode" will be send when the node is reached.
	AITRACKPAT_NODE_SIGNAL					= 0x04,

	/// If this flag is set, the advancing will be stopped. The advancing can be continued
	/// by changing the AgentParamters m_trackPatternContinue to true or by calling ContinueAdvance() of track pattern.
	AITRACKPAT_NODE_STOP						= 0x08,

	/// The default direction at each pattern node is direction from the node position to the center of the pattern.
	//	If this flag is set, the direction will be average direction to the branch nodes.
	AITRACKPAT_NODE_DIRBRANCH				= 0x10,
};


/// The validation method describes how the pattern is validated to fit the physical world:
enum ETrackPatternFlags
{
	/// No validation at all.
	AITRACKPAT_VALIDATE_NONE =					0x00,
	/// Validate using swept sphere tests, the sphere radius is validation radius plus the entity pass radius.
	AITRACKPAT_VALIDATE_SWEPTSPHERE =		0x01,
	/// Validate using raycasting, the hit position is pulled back by the amount of validation radius plus
	/// the entity pass radius.
	AITRACKPAT_VALIDATE_RAYCAST =				0x02,
	/// Align the pattern randonly each time it is set. The rotation ranges are set using SetRandomRotation().
	AITRACKPAT_ALIGN_RANDOM =						0x10,
	/// Align the pattern so that the y-axis will point towards the target each time the pattern is selected.
	AITRACKPAT_ALIGN_ORIENT_TO_TARGET =	0x20,
	/// Align the pattern so that it levels the target.
	AITRACKPAT_ALIGN_LEVEL_TO_TARGET =	0x40,
	/// Align the pattern so that the y-axis will point at the same directions as the target each time the pattern is selected.
	AITRACKPAT_ALIGN_TARGET_DIR =				0x80,
};


struct ITrackPatternDescriptorNode
{
	/// Set the node offset. If node flags 

	/// The offset from the start position or from the pattern center, see AITRACKPAT_NODE_ABSOLUTE.
	///	If zero offset is used, the node will become an alias, that is it will not be validated and
	///	the parent position and deformation value is used directly.
	virtual void	SetOffset( const Vec3& offset ) = 0;

	///	Sets the parent node name. If the parent name is set, the start position is considered to
	///	be the parent node position instead of the pattern center.
	virtual void	SetParentName( const char* parentName ) = 0;

	/// Sets the method to choose the next node when the node is reached.
	/// Should be one of the ETrackPatternBranchMethod.
	virtual void	SetBranchMethod( int branchMethod ) = 0;

	/// Adds node name to branch to in the branch list. The nodes in the list will be chosen based
	/// on the branch method.
	virtual void	AddBranchNodeName( const char* branchNodeName ) = 0;

	///	Defines the node evaluation flags, should be combination of ETrackPatternNodeFlags.
	virtual void	SetFlags( int flags ) = 0;

	///	This value will be passed as signal parameter if the AITRACKPAT_NODE_SIGNAL flag is set.
	/// The value is accessible from the signal handler in data.iValue.
	virtual void	SetSignalValue( int iValue ) = 0;
};


struct ITrackPatternDescriptor
{
	/// Returns the name of the pattern.
	virtual const char* GetName() const = 0;

	/// Sets the pattern functionality flags, see ETrackPatternFlags.
	virtual void	SetFlags( int flags ) = 0;

	/// Sets the pattern validation radius.
	virtual void	SetValidationRadius( float radius ) = 0;

	/// Sets the pattern validation radius.
	virtual void	SetAdvanceRadius( float radius ) = 0;

	/// The deformation of the pattern is tracked in global and local scale in range [0..1].
	/// These treshold values can be used to clamp the bottom range,
	/// so that values in range [trhd..1] becomes [0..1], default 0.0.
	virtual void	SetDeformationTreshold( float globalTreshold, float localTreshold ) = 0;

	/// Sets the global deformation state limits.
	///	stateTresholdMin - If the state of the pattern is 'enclosed' (high deformation) and
	///			the global deformation < stateTresholdMin, the state becomes exposed.
	///	stateTresholdMax - If the state of the pattern is 'exposed' (low deformation) and
	///			the global deformation > stateTresholdMax, the state becomes enclosed.
	virtual void	SetStateTreshold( float stateTresholdMin, float stateTresholdMax ) = 0;

	/// Sets the exposure modifier.
	/// The exposure modifier allows to take the node exposure (how much it is seen by the tracked target)
	/// into account when branching. The modifier should be in range [-1..1], where -1 means to favor unseen nodes,
	/// and 1 means to favor seen, exposed node and 0 means no effect.
	virtual void	SetExposureMod( float mod ) = 0;

	/// Sets the random rotation angles (in degrees) of the pattern.
	/// Each time the pattern is set, the rotation of the pattern is randomized.
	/// The amount of the rotation around each axis is controlled the values set by this method.
	/// The rotation order is XYZ, see Matrix33::SetRotationXYZ.
	virtual void	SetRandomRotation( const Ang3& angles ) = 0;

	/// Adds new node the pattern, see ITrackPatternDescriptorNode for descriptions of the parameters.
	virtual void	AddNode( const char* nodeName, const Vec3& offset, int flags, const char* parentName, int signalValue ) = 0;

	/// Returns pointer to node of speciafied name, or null if no node is found.
	virtual ITrackPatternDescriptorNode*				GetNode( const char* nodeName ) = 0;

	/// Returns const pointer to node of speciafied name, or null if no node is found.
	virtual const ITrackPatternDescriptorNode*	GetNode( const char* nodeName ) const = 0;

	/// Finalises the track pattern descriptor. Should be called after the all nodes and branches are setup,
	/// so that the pattern can be tidied up and optimized.
	virtual void	Finalize() = 0;
};


#endif __AITrackPatternDescriptor_H__
