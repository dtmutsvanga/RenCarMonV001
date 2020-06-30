package in.dc297.mqttclpro.activity;

import android.app.Application;
import android.content.Intent;
import android.os.StrictMode;
import android.preference.PreferenceActivity;
import android.support.design.widget.FloatingActionButton;
import android.support.multidex.MultiDexApplication;

import in.dc297.mqttclpro.BuildConfig;
import in.dc297.mqttclpro.R;
import in.dc297.mqttclpro.entity.BrokerEntity;
import in.dc297.mqttclpro.entity.Models;
import io.requery.Persistable;
import io.requery.android.sqlite.DatabaseSource;
import io.requery.cache.EntityCacheBuilder;
import io.requery.meta.EntityModel;
import io.requery.reactivex.ReactiveEntityStore;
import io.requery.reactivex.ReactiveSupport;
import io.requery.sql.Configuration;
import io.requery.sql.ConfigurationBuilder;
import io.requery.sql.EntityDataStore;

/**
 * Created by Deepesh on 10/16/2017.
 */

public class MQTTClientApplication extends MultiDexApplication {
    private ReactiveEntityStore<Persistable> dataStore;

    @Override
    public void onCreate() {
        super.onCreate();
        /*BrokerEntity broker1 = new BrokerEntity();
        broker1.setUsername("FlespiToken 46ZBRm8nLBMpvICsDTKfTgMLaN3PAv6spTR66PkHhWXAsnQV4qYNwAoucf8AB4SO");
        broker1.setHost("mqtt.flespi.io");
        broker1.setClientId("Test123");
        broker1.setPort("1883");
        broker1.setEnabled(true);*/
       // mqttClients.addBroker(broker1);

        /*Intent intent = new Intent(getApplicationContext(),AddEditBrokersActivity.class);
        intent.putExtra(PreferenceActivity.EXTRA_SHOW_FRAGMENT, AddEditBrokersActivity.GeneralPreferenceFragment.class.getName());
        intent.putExtra(PreferenceActivity.EXTRA_NO_HEADERS,true);
        startActivity(intent);
        StrictMode.enableDefaults();*/
    }

    public ReactiveEntityStore<Persistable> getData() {
        if (dataStore == null) {
            // override onUpgrade to handle migrating to a new version
            DatabaseSource source = new DatabaseSource(this, Models.DEFAULT,5);
            /*if (BuildConfig.DEBUG) {
                // use this in development mode to drop and recreate the tables on every upgrade
                source.setTableCreationMode(TableCreationMode.DROP_CREATE);
            }*/
            EntityModel model = Models.DEFAULT;
            Configuration configuration = new ConfigurationBuilder(source, model)
                    .useDefaultLogging()
                    .build();
            dataStore = ReactiveSupport.toReactiveStore(new EntityDataStore<Persistable>(configuration));
        }
        return dataStore;
    }
}
