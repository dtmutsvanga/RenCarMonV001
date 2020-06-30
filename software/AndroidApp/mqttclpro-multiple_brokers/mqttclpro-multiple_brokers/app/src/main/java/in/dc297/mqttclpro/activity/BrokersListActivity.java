package in.dc297.mqttclpro.activity;

import android.app.NotificationManager;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Build;
import android.os.Bundle;
import android.preference.PreferenceActivity;
import android.preference.PreferenceManager;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.TextInputEditText;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.Toolbar;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Toast;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import in.dc297.mqttclpro.BuildConfig;
import in.dc297.mqttclpro.R;
import in.dc297.mqttclpro.databinding.BrokerListItemBinding;
import in.dc297.mqttclpro.dialog.AddTopicDialogFragment;
import in.dc297.mqttclpro.dialog.AdsEnabledDialogFragment;
import in.dc297.mqttclpro.entity.BrokerEntity;
import in.dc297.mqttclpro.helpers.AdsHelper;
import in.dc297.mqttclpro.services.MyMqttService;
import io.reactivex.functions.Consumer;
import io.requery.Persistable;
import io.requery.android.QueryRecyclerAdapter;
import io.requery.query.Result;
import io.requery.reactivex.ReactiveEntityStore;

import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.AdView;
import com.google.android.gms.ads.MobileAds;

public class BrokersListActivity extends AppCompatActivity {

    private ReactiveEntityStore<Persistable> data;
    private ExecutorService executor;
    private BrokersListAdapter adapter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_brokers_list);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        setTitle(getResources().getString(R.string.title_activity_brokers_list));
        //start service
        Intent svc = new Intent(this, MyMqttService.class);
        startService(svc);

        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(getApplicationContext(),AddEditBrokersActivity.class);
                intent.putExtra(PreferenceActivity.EXTRA_SHOW_FRAGMENT, AddEditBrokersActivity.GeneralPreferenceFragment.class.getName());
                intent.putExtra(PreferenceActivity.EXTRA_NO_HEADERS,true);
                startActivity(intent);
            }
        });
        RecyclerView recyclerView = (RecyclerView) findViewById(R.id.recyclerView);
        data = ((MQTTClientApplication) getApplication()).getData();
        executor = Executors.newSingleThreadExecutor();
        adapter = new BrokersListAdapter();
        adapter.setExecutor(executor);
        recyclerView.setAdapter(adapter);
        recyclerView.setLayoutManager(new LinearLayoutManager(this));
        data.count(BrokerEntity.class).get().single()
                .subscribe(new Consumer<Integer>() {
                    @Override
                    public void accept(Integer integer) {
                        if (integer == 0) {
                            Toast.makeText(getApplicationContext(), "Please add a broker!",Toast.LENGTH_SHORT).show();
                        }
                    }
                });
        if(getAdsFirstTime()) new AdsEnabledDialogFragment().show(getFragmentManager(),"ADS_ENABLED_FRAGMENT");
        AdsHelper.initializeAds((AdView)findViewById(R.id.adView),this);

        FloatingActionButton fab2 = (FloatingActionButton) findViewById(R.id.fab);
        fab2.callOnClick();
        //AddEditBrokersActivity.
       // TextInputEditText txt = (TextInputEditText) findViewById(R.id.)

    }

    @Override
    protected void onResume() {
        adapter.queryAsync();
        super.onResume();
    }

    @Override
    protected void onDestroy() {
        executor.shutdown();
        adapter.close();
        super.onDestroy();
    }
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_brokers_list, menu);
        return true;
    }


    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        if(id == R.id.action_donate){
            Intent donateIntent = new Intent(this,DonationActivity.class);
            startActivity(donateIntent);
            return true;
        }
        if(id == R.id.action_settings){
            Intent settingsIntent = new Intent(this,SettingsActivity.class);
            startActivity(settingsIntent);
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    public class BrokersListAdapter extends QueryRecyclerAdapter<BrokerEntity, BindingHolder<BrokerListItemBinding>> implements View.OnClickListener, View.OnLongClickListener {

        BrokersListAdapter(){
            super(BrokerEntity.$TYPE);
        }
        @Override
        public void onClick(View v) {
            BrokerListItemBinding binding = (BrokerListItemBinding) v.getTag();
            if(binding!=null){
                Intent intent = new Intent(v.getContext(),SubscribedTopicsActivity.class);
                intent.putExtra(SubscribedTopicsActivity.EXTRA_BROKER_ID,binding.getBroker().getId());
                startActivity(intent);
            }
        }

        @Override
        public Result<BrokerEntity> performQuery() {
            return data.select(BrokerEntity.class).get();
        }

        @Override
        public void onBindViewHolder(BrokerEntity broker, BindingHolder<BrokerListItemBinding> brokerListItemBindingBindingHolder, int i) {
            brokerListItemBindingBindingHolder.binding.setBroker(broker);
        }

        @Override
        public BindingHolder<BrokerListItemBinding> onCreateViewHolder(ViewGroup parent, int viewType) {
            LayoutInflater inflater  = LayoutInflater.from(parent.getContext());
            BrokerListItemBinding binding = BrokerListItemBinding.inflate(inflater, parent, false);
            binding.getRoot().setTag(binding);
            binding.getRoot().setOnClickListener(this);
            binding.getRoot().setLongClickable(true);
            binding.getRoot().setOnLongClickListener(this);
            return new BindingHolder<>(binding);
        }

        @Override
        public boolean onLongClick(View v) {
            BrokerListItemBinding binding = (BrokerListItemBinding) v.getTag();
            if(binding!=null){
                Intent intent = new Intent(v.getContext(),AddEditBrokersActivity.class);
                intent.putExtra(AddEditBrokersActivity.EXTRA_BROKER_ID,binding.getBroker().getId());
                //Toast.makeText(v.getContext(),binding.getBroker().toString(),Toast.LENGTH_SHORT).show();
                startActivity(intent);
            }
            return true;
        }
    }

    private boolean getAdsFirstTime(){
        SharedPreferences mSharedPreferences = PreferenceManager.getDefaultSharedPreferences(getApplicationContext());
        boolean flag = mSharedPreferences.getBoolean(getString(R.string.ads_first),true);
        if(flag) {
            SharedPreferences.Editor mSharedPrefsEditor = mSharedPreferences.edit();
            mSharedPrefsEditor.putBoolean(getString(R.string.ads_first), false);
            mSharedPrefsEditor.commit();
        }
        return flag;
    }

}