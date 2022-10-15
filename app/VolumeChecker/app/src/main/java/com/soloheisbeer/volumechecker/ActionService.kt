package com.soloheisbeer.volumechecker


import android.app.*
import android.content.Intent
import android.content.IntentFilter
import android.hardware.display.DisplayManager
import android.media.AudioManager
import android.net.ConnectivityManager
import android.net.ConnectivityManager.NetworkCallback
import android.net.Network
import android.os.IBinder
import android.os.PowerManager
import android.util.Log
import android.view.Display


class ActionService : Service() {

    companion object {
        const val NOTIFICATION_ID = 101
        const val CHANNEL_ID = "ActionObserver"
        const val CHANNEL_NAME = "ActionObserver"
    }

    private lateinit var audioManager: AudioManager
    private lateinit var powerManager: PowerManager
    private lateinit var displayManager: DisplayManager
    private lateinit var connectivityManager: ConnectivityManager


    private val commandSender: CommandSender = CommandSender()
    private var actionBroadCastReceiver: ActionBroadCastReceiver? = null

    private val networkCallback: NetworkCallback = object : NetworkCallback() {
        override fun onAvailable(network: Network) {
            super.onAvailable(network)
            Log.d("STATE:", "Network connected")
            if (powerManager.isInteractive && displayManager.displays[0].state == Display.STATE_ON) {
                commandSender.powerOn()
            }
        }
    }


    override fun onStartCommand(intent: Intent, flags: Int, startId: Int): Int {
        Log.d(packageName, "START SERVICE")

        audioManager = getSystemService(AudioManager::class.java)
        powerManager = getSystemService(PowerManager::class.java)
        displayManager = getSystemService(DisplayManager::class.java)
        connectivityManager = getSystemService(ConnectivityManager::class.java)

        unregisterReceiver()

        if (powerManager.isInteractive && displayManager.displays[0].state == Display.STATE_ON) {
            commandSender.powerOn()
        }

        registerReceiver(audioManager, displayManager, commandSender)
        connectivityManager.registerDefaultNetworkCallback(networkCallback)

        startForeground(NOTIFICATION_ID, createNotification())

        return START_STICKY
    }

    override fun onDestroy() {
        super.onDestroy()
        unregisterReceiver()
        connectivityManager.unregisterNetworkCallback(networkCallback)
    }

    private fun createNotification(): Notification {

        val serviceChannel = NotificationChannel(
            CHANNEL_ID, CHANNEL_NAME,
            NotificationManager.IMPORTANCE_LOW
        )

        val notificationManager = getSystemService(NotificationManager::class.java)!!
        notificationManager.createNotificationChannel(serviceChannel)


        val notificationIntent = Intent(this, MainActivity::class.java)
        val pendingIntent: PendingIntent? = TaskStackBuilder.create(applicationContext).run {
            addNextIntent(notificationIntent)
            getPendingIntent(
                0,
                PendingIntent.FLAG_CANCEL_CURRENT + PendingIntent.FLAG_IMMUTABLE
            )
        }

        return Notification.Builder(this, CHANNEL_ID)
            .setContentTitle(getString(R.string.app_name))
            .setContentText(getText(R.string.notification_message))
            .setSmallIcon(R.drawable.best2)
            .setContentIntent(pendingIntent)
            .build()
    }

    private fun registerReceiver(
        audioManager: AudioManager,
        displayManager: DisplayManager,
        commandSender: CommandSender
    ) {
        actionBroadCastReceiver = ActionBroadCastReceiver(
            audioManager,
            displayManager,
            commandSender
        )
        val filter = IntentFilter()
        filter.addAction(ActionBroadCastReceiver.SCREEN_ON)
        filter.addAction(ActionBroadCastReceiver.SCREEN_OFF)
        filter.addAction(ActionBroadCastReceiver.VOLUME_CHANGE_ACTION)
        if (actionBroadCastReceiver != null) {
            applicationContext.registerReceiver(actionBroadCastReceiver!!, filter)
        }
    }

    private fun unregisterReceiver() {
        if (actionBroadCastReceiver != null) {
            applicationContext.unregisterReceiver(actionBroadCastReceiver!!)
            actionBroadCastReceiver = null
        }
    }

    override fun onBind(intent: Intent?): IBinder? {
        return null
    }
}