package managers;

import android.os.Bundle;
import android.util.Log;

import com.google.firebase.analytics.FirebaseAnalytics;
import org.libsdl.app.SDLActivity;

public class GoogleAnalyticsManager {

    private static FirebaseAnalytics firebaseAnalytics;

    public static void init(SDLActivity activity) {
        Log.v("GoogleAnalyticsManager", "init(SDLActivity activity)");
        firebaseAnalytics = FirebaseAnalytics.getInstance(activity);
    }

    // Called from C++ code.
    public static void sendEventEmpty(String eventType) {
        Log.v("GoogleAnalyticsManager", "sendEventEmpty(): " + eventType);

        Bundle bundle = new Bundle();

        firebaseAnalytics.logEvent(eventType, bundle);
    }

    public static void sendEventStringParam(String eventType, String paramName, String paramValue) {
        // Example: sendEventStringParam("level_start", "level_name", "first");
        Log.v("GoogleAnalyticsManager", "sendEventStringParam(): " + eventType + " " + paramName + " " + paramValue);

        Bundle bundle = new Bundle();
        bundle.putString(paramName, paramValue);

        firebaseAnalytics.logEvent(eventType, bundle);
    }

    public static void sendEventIntParam(String eventType, String paramName, int paramValue) {
        // Example: sendEventIntParam("player_die", "die_time", 145);
        Log.v("GoogleAnalyticsManager", "sendEventIntParam(): " + eventType + " " + paramName + " " + paramValue);

        Bundle bundle = new Bundle();
        bundle.putInt(paramName, paramValue);

        firebaseAnalytics.logEvent(eventType, bundle);
    }
}
