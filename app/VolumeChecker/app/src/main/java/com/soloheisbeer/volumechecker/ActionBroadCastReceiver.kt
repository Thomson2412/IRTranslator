package com.soloheisbeer.volumechecker

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.media.AudioManager
import android.util.Log
import android.widget.Toast


class ActionBroadCastReceiver(private val networkManager: NetworkManager) : BroadcastReceiver() {

    companion object {
        const val SCREEN_ON = "android.intent.action.SCREEN_ON"
        const val SCREEN_OFF = "android.intent.action.SCREEN_OFF"
        const val VOLUME_CHANGE_ACTION = "android.media.VOLUME_CHANGED_ACTION"
        const val EXTRA_VOLUME_STREAM_TYPE = "android.media.EXTRA_VOLUME_STREAM_TYPE"
    }

    override fun onReceive(context: Context?, intent: Intent?) {
        if (intent?.action == VOLUME_CHANGE_ACTION && intent.getIntExtra(
                EXTRA_VOLUME_STREAM_TYPE,
                -1
            ) == AudioManager.STREAM_MUSIC
        ) {
            val newVolume = intent.getIntExtra("android.media.EXTRA_VOLUME_STREAM_VALUE", -1)
            val oldVolume = intent.getIntExtra("android.media.EXTRA_PREV_VOLUME_STREAM_VALUE", -1)

            if (newVolume > oldVolume || newVolume == 100) {
                Log.d("ACTION:", "UP")
                Toast.makeText(context, "Up", Toast.LENGTH_SHORT).show()
                networkManager.volumeUp()
            } else if (newVolume < oldVolume || newVolume == 0) {
                Log.d("ACTION:", "DOWN")
                Toast.makeText(context, "Down", Toast.LENGTH_SHORT).show()
                networkManager.volumeDown()
            }
        }

        if (intent?.action == Intent.ACTION_SCREEN_ON) {
            Log.d("ACTION", "ON")
            Toast.makeText(context, "On", Toast.LENGTH_SHORT).show()
            networkManager.powerOn()
        } else if (intent?.action == Intent.ACTION_SCREEN_OFF) {
            Log.d("ACTION", "OFF")
            Toast.makeText(context, "Off", Toast.LENGTH_SHORT).show()
            networkManager.powerOff()
        }
    }

}