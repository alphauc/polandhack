#pragma once

#include "material.hh"
#include "../misc/vector.hh"

template <class T>
static T im_a_lazy_fuck_again( void* instance, int index )
{
	const auto vtable = *static_cast< void*** >( instance );
	return reinterpret_cast< T >( vtable[ index ] );
}

namespace sdk
{
	struct model_t;

	struct mstudioanimdesc_t {};

	struct mstudioseqdesc_t {};

	enum HitboxList
	{
		HITBOX_PELVIS,
		HITBOX_L_THIGH,
		HITBOX_L_CALF,
		HITBOX_L_FOOT,
		HITBOX_R_THIGH,
		HITBOX_R_CALF,
		HITBOX_R_FOOT,
		HITBOX_SPINE1,
		HITBOX_SPINE2,
		HITBOX_SPINE3,
		HITBOX_NECK,
		HITBOX_HEAD,
		HITBOX_L_UPPERARM,
		HITBOX_L_FOREARM,
		HITBOX_L_HAND,
		HITBOX_R_UPPERARM,
		HITBOX_R_FOREARM,
		HITBOX_R_HAND,
		HITBOX_L_CLAVICLE,
		HITBOX_R_CLAVICLE,
		HITBOX_SPINE4,
		HITBOX_MAX,
	};

	struct mstudiobbox_t
	{
		int bone;
		int group;
		vec3 bbmin;
		vec3 bbmax;
		int szhitboxnameindex;
		int m_iPad01[ 3 ];
		float m_flRadius;
		int m_iPad02[ 4 ];
	};

	struct mstudiobone_t
	{
		int sznameindex;

		inline char* const pszName( void ) const
		{
			return ( ( char * )this ) + sznameindex;
		}

		int parent; // parent bone
		int bonecontroller[ 6 ]; // bone controller index, -1 == none

		// default values
		vec3 pos;
		quaternion quat;
		vec3 rot;
		// compression scale
		vec3 posscale;
		vec3 rotscale;

		v_matrix poseToBone;
		quaternion qAlignment;
		int flags;
		int proctype;
		int procindex; // procedural rule
		mutable int physicsbone; // index into physically simulated bone
		inline void* pProcedure( ) const
		{
			if ( procindex == 0 )
				return NULL;
			else
				return ( void * )( ( ( unsigned char * )this ) + procindex );
		};
		int surfacepropidx; // index into string tablefor property name
		inline char* const pszSurfaceProp( void ) const
		{
			return ( ( char * )this ) + surfacepropidx;
		}

		int contents; // See BSPFlags.h for the contents flags

		int unused[ 8 ]; // remove as appropriate

		mstudiobone_t( ) {}
	private:
		// No copy constructors allowed
		mstudiobone_t( const mstudiobone_t& vOther );
	};

	struct mstudiohitboxset_t
	{
		int sznameindex;

		inline char* const GetName( void ) const
		{
			return ( ( char* )this ) + sznameindex;
		}

		int numhitboxes;
		int hitboxindex;

		inline mstudiobbox_t* GetHitbox( int i ) const
		{
			return ( mstudiobbox_t* )( ( ( unsigned char* )this ) + hitboxindex ) + i;
		};
	};

	struct studiohdr_t
	{
		int id;
		int version;

		int checksum;

		char name[ 64 ];
		int length;

		vec3 eyeposition;

		vec3 illumposition;

		vec3 hull_min;
		vec3 hull_max;

		vec3 view_bbmin;
		vec3 view_bbmax;

		int flags;

		int numbones;
		int boneindex;

		inline mstudiobone_t* GetBone( int i ) const
		{
			return ( mstudiobone_t * )( ( ( unsigned char * )this ) + boneindex ) + i;
		};
		//	inline mstudiobone_t *pBone(int i) const { Assert(i >= 0 && i < numbones); return (mstudiobone_t *)(((byte *)this) + boneindex) + i; };

		int numbonecontrollers;
		int bonecontrollerindex;

		int numhitboxsets;
		int hitboxsetindex;

		mstudiohitboxset_t* GetHitboxSet( int i ) const
		{
			return ( mstudiohitboxset_t* )( ( ( unsigned char* )this ) + hitboxsetindex ) + i;
		}

		inline mstudiobbox_t* GetHitbox( int i, int set ) const
		{
			mstudiohitboxset_t const* s = GetHitboxSet( set );

			if ( !s )
				return NULL;

			return s->GetHitbox( i );
		}

		inline int GetHitboxCount( int set ) const
		{
			mstudiohitboxset_t const* s = GetHitboxSet( set );

			if ( !s )
				return 0;

			return s->numhitboxes;
		}

		int numlocalanim;
		int localanimindex;

		int numlocalseq;
		int localseqindex;

		mutable int activitylistversion;
		mutable int eventsindexed;

		int numtextures;
		int textureindex;

		int numcdtextures;
		int cdtextureindex;

		int numskinref;
		int numskinfamilies;
		int skinindex;

		int numbodyparts;
		int bodypartindex;

		int numlocalattachments;
		int localattachmentindex;

		int numlocalnodes;
		int localnodeindex;
		int localnodenameindex;

		int numflexdesc;
		int flexdescindex;

		int numflexcontrollers;
		int flexcontrollerindex;

		int numflexrules;
		int flexruleindex;

		int numikchains;
		int ikchainindex;

		int nummouths;
		int mouthindex;

		int numlocalposeparameters;
		int localposeparamindex;

		int surfacepropindex;

		int keyvalueindex;
		int keyvaluesize;

		int numlocalikautoplaylocks;
		int localikautoplaylockindex;

		float mass;
		int contents;

		int numincludemodels;
		int includemodelindex;

		mutable void* virtualModel;

		int szanimblocknameindex;
		int numanimblocks;
		int animblockindex;

		mutable void* animblockModel;

		int bonetablebynameindex;

		void* pVertexBase;
		void* pIndexBase;

		unsigned char constdirectionallightdot;

		unsigned char rootLOD;

		unsigned char numAllowedRootLODs;

		unsigned char unused[ 1 ];

		int unused4;

		int numflexcontrollerui;
		int flexcontrolleruiindex;
		float flVertAnimFixedPointScale;
		int unused3[ 1 ];
		int studiohdr2index;
		int unused2[ 1 ];
	};

	//0x3F00FB33
	class model_information
	{
	public:
		studiohdr_t* GetStudioModel( const model_t* Model )
		{
			typedef studiohdr_t*( __thiscall* Fn )( void*, const model_t* );
			return im_a_lazy_fuck_again<Fn>( this, 32 )( this, Model );
		}

		int GetModelIndex( const char* model )
		{
			typedef int( __thiscall* Fn )( void*, const char* );
			return im_a_lazy_fuck_again<Fn>( this, 4 )( this, model );
		}

		char* GetModelName( const model_t* Model )
		{
			typedef char*( __thiscall* Fn )( void*, const model_t* );
			return im_a_lazy_fuck_again<Fn>( this, 3 )( this, Model );
		}

		void GetModelMaterials( const model_t* model, int count, material** ppMaterial )
		{
			typedef char*( __thiscall* Fn )( void*, const model_t*, int, material** );
			im_a_lazy_fuck_again<Fn>( this, 19 )( this, model, count, ppMaterial );
		}
	};
}
