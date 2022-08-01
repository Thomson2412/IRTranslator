package com.soloheisbeer.volumechecker


import android.app.*
import android.content.Intent
import android.content.IntentFilter
import android.os.IBinder
import android.util.Log


class ActionService : Service() {

    companion object{
        const val NOTIFICATION_ID = 101
        const val CHANNEL_ID = "ActionObserver"
        const val CHANNEL_NAME = "ActionObserver"
    }

    private var actionBroadCastReceiver: ActionBroadCastReceiver? = null

    override fun onStartCommand(intent: Intent, flags: Int, startId: Int): Int {
        Log.d(packageName,"START SERVICE")
        registerReceiver(NetworkManager())

        startForeground(NOTIFICATION_ID, createNotification())

        return START_STICKY
    }

    override fun onDestroy() {
        super.onDestroy()
        unregisterReceiver()
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

    private fun registerReceiver(networkManager: NetworkManager) {
        actionBroadCastReceiver = ActionBroadCastReceiver(networkManager)
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