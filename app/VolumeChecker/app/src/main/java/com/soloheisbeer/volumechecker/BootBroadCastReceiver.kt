package com.soloheisbeer.volumechecker

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent

class BootBroadCastReceiver : BroadcastReceiver() {

    override fun onReceive(context: Context?, intent: Intent?) {
        val startIntent = Intent(context, ActionService::class.java)
        startIntent.setPackage(context?.packageName)
        context?.startForegroundService(intent)
    }
}