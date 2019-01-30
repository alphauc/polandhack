#pragma once
#include "../misc/vector.hh"
#include "../classes/user_cmd.hh"
#include "../classes/entity.hh"

template<typename FuncType>
__forceinline static FuncType call_virtual( void* ppClass, int index ) { // lazy piece of shit
	int* pVTable = *( int** )ppClass;
	int dwAddress = pVTable[ index ];
	return ( FuncType )( dwAddress );
}


namespace sdk {
	class player_move_helper
	{
	public:

		bool bFirstRunOfFunctions : 1;
		bool bGameCodeMovedPlayer : 1;
		int nPlayerHandle; // edict index on server, client entity handle on client=
		int nImpulseCommand; // Impulse command issued.
		vec3 vecViewAngles; // Command view angles (local space)
		vec3 vecAbsViewAngles; // Command view angles (world space)
		int nButtons; // Attack buttons.
		int nOldButtons; // From host_client->oldbuttons;
		float flForwardMove;
		float flSideMove;
		float flUpMove;
		float flMaxSpeed;
		float flClientMaxSpeed;
		vec3 vecVelocity; // edict::velocity // Current movement direction.
		vec3 vecAngles; // edict::angles
		vec3 vecOldAngles;
		float outStepHeight; // how much you climbed this move
		vec3 outWishVel; // This is where you tried 
		vec3 outJumpVel; // This is your jump velocity
		vec3 vecConstraintCenter;
		float flConstraintRadius;
		float flConstraintWidth;
		float flConstraintSpeedFactor;
		float flUnknown[ 5 ];
		vec3 vecAbsOrigin;

		virtual	void _vpad( ) = 0;
		virtual void set_host( base_entity* host ) = 0;
	};

	class player_move_data {
	public:
		bool    bFirstRunOfFunctions : 1;
		bool    bGameCodeMovedPlayer : 1;
		int     nPlayerHandle;        // edict index on server, client entity handle on client=
		int     nImpulseCommand;      // Impulse command issued.
		vec3    vecViewAngles;        // Command view angles (local space)
		vec3  vecAbsViewAngles;     // Command view angles (world space)
		int     nButtons;             // Attack buttons.
		int     nOldButtons;          // From host_client->oldbuttons;
		float   flForwardMove;
		float   flSideMove;
		float   flUpMove;
		float   flMaxSpeed;
		float   flClientMaxSpeed;
		vec3  vecVelocity;          // edict::velocity        // Current movement direction.
		vec3  vecAngles;            // edict::angles
		vec3  vecOldAngles;
		float   outStepHeight;        // how much you climbed this move
		vec3  outWishVel;           // This is where you tried 
		vec3  outJumpVel;           // This is your jump velocity
		vec3  vecConstraintCenter;
		float   flConstraintRadius;
		float   flConstraintWidth;
		float   flConstraintSpeedFactor;
		float   flUnknown[ 5 ];
		vec3  vecAbsOrigin;
	};

	class virtual_game_movement {
	public:
		virtual			~virtual_game_movement( void ) { }

		virtual void	          process_movement( base_entity *pPlayer, player_move_data *pMove ) = 0;
		virtual void	          Reset( void ) = 0;
		virtual void	          start_track_prediction_errors( base_entity *pPlayer ) = 0;
		virtual void	          finish_track_prediction_errors( base_entity *pPlayer ) = 0;
		virtual void	          DiffPrint( char const *fmt, ... ) = 0;
		virtual vec3 const&	  GetPlayerMins( bool ducked ) const = 0;
		virtual vec3 const&	  GetPlayerMaxs( bool ducked ) const = 0;
		virtual vec3 const&   GetPlayerViewOffset( bool ducked ) const = 0;
		virtual bool		        IsMovingPlayerStuck( void ) const = 0;
		virtual base_entity*   GetMovingPlayer( void ) const = 0;
		virtual void		        UnblockPusher( base_entity *pPlayer, base_entity *pPusher ) = 0;
		virtual void            SetupMovementBounds( player_move_data *pMove ) = 0;
	};

	class player_game_movement
		: public virtual_game_movement {
	public:
		virtual ~player_game_movement( void ) { }
	};

	class player_prediction {
	public:
		bool in_prediction( ) {
			typedef bool( __thiscall* oInPrediction )( void* );
			return call_virtual<oInPrediction>( this, 14 )( this );
		}

		void run_command( base_entity *player, user_cmd *ucmd, player_move_helper *moveHelper ) {
			typedef void( __thiscall* oRunCommand )( void*, base_entity*, user_cmd*, player_move_helper* );
			return call_virtual<oRunCommand>( this, 19 )( this, player, ucmd, moveHelper );
		}

		void setup_move( base_entity *player, user_cmd *ucmd, player_move_helper *moveHelper, void* pMoveData ) {
			typedef void( __thiscall* oSetupMove )( void*, base_entity*, user_cmd*, player_move_helper*, void* );
			return call_virtual<oSetupMove>( this, 20 )( this, player, ucmd, moveHelper, pMoveData );
		}

		void finish_move( base_entity *player, user_cmd *ucmd, void*pMoveData ) {
			typedef void( __thiscall* oFinishMove )( void*, base_entity*, user_cmd*, void* );
			return call_virtual<oFinishMove>( this, 21 )( this, player, ucmd, pMoveData );
		}
	};
}
