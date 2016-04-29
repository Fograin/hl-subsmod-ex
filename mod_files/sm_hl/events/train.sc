// System-wide global variables
$server = false;
$localplayer = true;
$origin = [ 0, 0, 0 ];
$view_ofs = [ 0, 0, 0 ];
$angles = [ 0, 0, 0 ];
$gun_position [ 0, 0, 0 ];
$gun_angles[ 0, 0, 0 ];
$usehull = 0;
$client_time = 0;

// 
$snd = "";
$model = "";
$shell_origin = [ 0, 0, 0 ];
$shell_velocity = [ 0, 0, 0 ];
$forward = [ 0, 0, 0 ];
$right   = [ 0, 0, 0 ];
$up		 = [ 0, 0, 0 ];

public Fire_Glock()
{
	// Play a sound
	$snd = weapons/pl_gun3.wav;
	$model = models/shell.mdl;

	Native_PlayPlayerSound( $player = $localplayer, $sound = $snd, $channel = CHAN_STATIC, $pitch = $def_pitch, $attenuation = $def_attn, $pitch = $def_pitch );

	// Eject shells
	//
	for ( $i = 0; $i < 2; $i++ )
	{
		$shell_origin   = $origin + $view_ofs;
		$shell_velocity = $forward * 24 + right * 6 + $up * 20;

		$shell_angles = $angles;
		$angles[0] = 0;
		$angles[2] = 0;

		Native_CreateTempEntity
		(
			$te_origin = $shell_origin,
			$te_angles = $angles,
			$te_framemax = Native_FrameCountForModel( $model = $model ),
			$te_flags = FTENT_COLLIDEWORLD | FTENT_GRAVITY | FTENT_ROTATE,
			$te_hitSound = BOUNCE_SHELL;
			$te_baselineangles = Native_RandomAngles(),
			$te_basedirection  = $shell_velocity,
			$te_die = $client_time + 3.5,
			$te_body = Native_Random( 0, $te_framemax )
		)

		// Now fire bullets
		$start = $origin;
		$end = $origin + 8192 * $gun_angles;

		Native_PerformTrace( $start = $start, $end = $end, $flags = PM_STUDIO_BOX );

		if ( $trace_fraction != 1.0 )
		{
			Native_PlaySoundAtPosition( $origin = $trace_endpos, $sound = weapons/bullet_hit1.wav );
			Native_ImpactParticles( $origin = $trace_endpos );
			Native_PlaceDecal( $origin = $trace_endpos, $decal = "{shot2", $trace_entity );
		}
	}
}