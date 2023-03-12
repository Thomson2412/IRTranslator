package com.soloheisbeer.volumechecker

import android.content.Context
import android.hardware.display.DisplayManager
import android.os.PowerManager
import android.provider.Settings
import android.util.Log
import android.view.Display

object Utils {

    fun isScreenOn(context: Context): Boolean {
        val powerManager = context.getSystemService(PowerManager::class.java)
        val displayManager = context.getSystemService(DisplayManager::class.java)
        val brightness =
            Settings.System.getInt(context.contentResolver, Settings.System.SCREEN_BRIGHTNESS)
        Log.d("Utils:", brightness.toString())
        if (powerManager.isInteractive &&
            displayManager.displays[0].state == Display.STATE_ON &&
            brightness > 0) {
            return true
        }
        return false
    }

}