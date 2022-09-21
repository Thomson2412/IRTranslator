package com.soloheisbeer.volumechecker

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.media.AudioManager
import android.net.ConnectivityManager
import android.net.ConnectivityManager.NetworkCallback
import android.net.Network
import android.util.Log


class ActionBroadCastReceiver(
    private val audioManager: AudioManager,
    private val connectivityManager: ConnectivityManager,
    private val commandSender: CommandSender) : BroadcastReceiver() {

    companion object {
        const val SCREEN_ON = "android.intent.action.SCREEN_ON"
        const val SCREEN_OFF = "android.intent.action.SCREEN_OFF"
        const val VOLUME_CHANGE_ACTION = "android.media.VOLUME_CHANGED_ACTION"
        const val EXTRA_VOLUME_STREAM_TYPE = "android.media.EXTRA_VOLUME_STREAM_TYPE"
    }

    val networkCallback: NetworkCallback = object : NetworkCallback() {
        override fun onAvailable(network: Network) {
            super.onAvailable(network)
            if (turnOnWhenConnected){
                commandSender.powerOn()
                turnOnWhenConnected = false
            }
        }
    }

    private var turnOnWhenConnected = false
    private var isMute = audioManager.isStreamMute(AudioManager.STREAM_MUSIC)


    override fun onReceive(context: Context?, intent: Intent?) {
        val streamType = intent?.getIntExtra(EXTRA_VOLUME_STREAM_TYPE, -1)
        if (intent?.action == VOLUME_CHANGE_ACTION && streamType == AudioManager.STREAM_MUSIC) {
            val newVolume = intent.getIntExtra("android.media.EXTRA_VOLUME_STREAM_VALUE", -1)
            val oldVolume = intent.getIntExtra("android.media.EXTRA_PREV_VOLUME_STREAM_VALUE", -1)

            Log.d("ACTION:", "Change - old: $oldVolume - new: $newVolume")
            val newMute = audioManager.isStreamMute(AudioManager.STREAM_MUSIC)
            if (isMute != newMute){
                Log.d("ACTION:", "MUTE")
                isMute = newMute
                commandSender.mute()
            }

            if (newVolume > oldVolume || newVolume == 100) {
                Log.d("ACTION:", "UP")
                commandSender.volumeUp(newVolume - oldVolume)
            } else if (newVolume < oldVolume || newVolume == 0) {
                Log.d("ACTION:", "DOWN")
                commandSender.volumeDown(oldVolume - newVolume)
            }
        }

        if (intent?.action == Intent.ACTION_SCREEN_ON) {
            Log.d("ACTION", "ON")
            if (connectivityManager.isDefaultNetworkActive) {
                commandSender.powerOn()
            } else{
                turnOnWhenConnected = true
            }

        } else if (intent?.action == Intent.ACTION_SCREEN_OFF) {
            Log.d("ACTION", "OFF")
            commandSender.powerOff()
            turnOnWhenConnected = false
        }
    }
}