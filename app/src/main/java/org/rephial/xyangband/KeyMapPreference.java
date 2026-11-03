package org.rephial.xyangband;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.os.Parcelable;
import android.preference.DialogPreference;
import android.util.AttributeSet;
import android.view.KeyEvent;

public class KeyMapPreference
	extends DialogPreference implements DialogInterface.OnClickListener {

	protected Context context;
	protected boolean alt_mod = false;
	protected boolean char_mod = false;
    protected int key_code = 0;

	public KeyMapPreference(Context context, AttributeSet attrs) {
		super(context, attrs);
		this.context = context;
		setDialogTitle(context.getString(R.string.keymap_press_hardware_key));
		setPositiveButtonText(context.getString(R.string.keymap_clear_assignment));
		setNegativeButtonText(context.getString(R.string.action_cancel));
	}
	@Override
    protected void onPrepareDialogBuilder(AlertDialog.Builder builder) {
		super.onPrepareDialogBuilder(builder);

		alt_mod = false;
		char_mod = false;
		key_code = 0;

		builder.setOnKeyListener(new DialogInterface.OnKeyListener() {
			public boolean onKey(DialogInterface dialog, int keyCode, KeyEvent event) {

				if (event.getAction() != KeyEvent.ACTION_DOWN
					|| event.getRepeatCount()>0)
					return true;

				if (handleModifier(keyCode))
					return true;

				saveKeyAssignment(keyCode, event);
				dialog.dismiss();
				return true;
			}
	   });
	}

	protected boolean handleModifier(int keyCode) {
		switch(keyCode) {
		case KeyEvent.KEYCODE_FOCUS: // eat focus for now (light camera press)
		case KeyEvent.KEYCODE_SHIFT_LEFT: // eat shift for now
		case KeyEvent.KEYCODE_SHIFT_RIGHT:
			return true;
		case KeyEvent.KEYCODE_ALT_LEFT: // allow alt
		case KeyEvent.KEYCODE_ALT_RIGHT:
			alt_mod = !alt_mod;
			return true;
		default:
			return false;
		}
	}

	protected void saveKeyAssignment(int keyCode, KeyEvent event) {
		String oldValue = getSharedPreferences().getString(getKey(),"");

		if (event.isAltPressed()) alt_mod = true;

		int meta=0;
		if(alt_mod) {
			meta |= KeyEvent.META_ALT_ON;
			meta |= KeyEvent.META_ALT_LEFT_ON;
		}
		int ch = event.getUnicodeChar(meta);
		char_mod = (ch > 32 && ch < 127);
		key_code = char_mod ? ch : keyCode;

		if (key_code == KeyEvent.KEYCODE_BACK){
			new AlertDialog.Builder(context)
				.setTitle("Angband")
				.setMessage(R.string.keymap_confirm_back_key)
				.setNegativeButton(R.string.action_cancel, new DialogInterface.OnClickListener() {
						public void onClick(DialogInterface dialog, int whichButton) {
						}
					})
				.setPositiveButton(R.string.action_ok, new DialogInterface.OnClickListener() {
						public void onClick(DialogInterface dialog, int whichButton) {
							saveMap();
						}
					})
				.show();
		} else {
			saveMap();
		}
	}

	public void saveMap() {
		KeyMap prevMap = Preferences.getKeyMapper()
			.assignKeyMap(getKey(), key_code, alt_mod, char_mod);

		SharedPreferences.Editor ed = getSharedPreferences().edit();
		if (prevMap != null) ed.putString(prevMap.getPrefKey(), "");
		ed.putString(getKey(), getValue());

		ed.commit();
	}

	public String getValue() {
		if (key_code == 0)
			return "";
		else
			return KeyMap.stringValue(key_code,alt_mod,char_mod);
	}

	public String getDescription() {
		SharedPreferences settings =  getSharedPreferences();
		String val = settings.getString(getKey(),"");

		String desc = context.getString(R.string.keymap_unassigned);

		if (val != null && val.length()>0) {
			if (val.startsWith("C"))
				desc = val.substring(1).toUpperCase();
			else
				desc = keyCodeDescription(Integer.parseInt(val),val.startsWith("0"));
		}

		return desc;
	}

	protected String keyCodeDescription(int code, boolean alt) {
		switch (code) {
		case KeyEvent.KEYCODE_ALT_LEFT:
			return context.getString(R.string.keymap_key_left_alt);
		case KeyEvent.KEYCODE_ALT_RIGHT:
			return context.getString(R.string.keymap_key_right_alt);
		case KeyEvent.KEYCODE_SHIFT_LEFT:
			return context.getString(R.string.keymap_key_left_shift);
		case KeyEvent.KEYCODE_SHIFT_RIGHT:
			return context.getString(R.string.keymap_key_right_shift);
		case KeyEvent.KEYCODE_CAMERA:
			return context.getString(R.string.keymap_key_camera);
		case KeyEvent.KEYCODE_SEARCH:
			return context.getString(R.string.keymap_key_search);
		case KeyEvent.KEYCODE_BACK:
			return context.getString(R.string.keymap_key_back);
		case KeyEvent.KEYCODE_MENU:
			return context.getString(R.string.keymap_key_menu);
		case KeyEvent.KEYCODE_DEL:
			return context.getString(R.string.keymap_key_backspace);
		case KeyEvent.KEYCODE_SPACE:
			return context.getString(R.string.keymap_key_space);
		case KeyEvent.KEYCODE_VOLUME_UP:
			return context.getString(R.string.keymap_key_volume_up);
		case KeyEvent.KEYCODE_VOLUME_DOWN:
			return context.getString(R.string.keymap_key_volume_down);
		case KeyEvent.KEYCODE_MOVE_END:
			return context.getString(R.string.keymap_key_end);
		case KeyEvent.KEYCODE_MOVE_HOME:
			return context.getString(R.string.keymap_key_home);
		case KeyEvent.KEYCODE_DPAD_CENTER:
			return context.getString(R.string.keymap_key_dpad_center);
		case KeyEvent.KEYCODE_DPAD_LEFT:
			return context.getString(R.string.keymap_key_dpad_left);
		case KeyEvent.KEYCODE_DPAD_RIGHT:
			return context.getString(R.string.keymap_key_dpad_right);
		case KeyEvent.KEYCODE_DPAD_DOWN:
			return context.getString(R.string.keymap_key_dpad_down);
		case KeyEvent.KEYCODE_DPAD_UP:
			return context.getString(R.string.keymap_key_dpad_up);
		case KeyEvent.KEYCODE_SOFT_LEFT:
			return context.getString(R.string.keymap_key_softkey_left);
		case KeyEvent.KEYCODE_SOFT_RIGHT:
			return context.getString(R.string.keymap_key_softkey_right);
		case KeyEvent.KEYCODE_SYM:
			return context.getString(R.string.keymap_key_sym);
		case KeyEvent.KEYCODE_TAB:
			return context.getString(R.string.keymap_key_tab);
		case KeyEvent.KEYCODE_PAGE_DOWN:
			return context.getString(R.string.keymap_key_page_down);
		case KeyEvent.KEYCODE_PAGE_UP:
			return context.getString(R.string.keymap_key_page_up);
		case KeyEvent.KEYCODE_NOTIFICATION:
			return context.getString(R.string.keymap_key_notification);
		case KeyEvent.KEYCODE_MUTE:
			return context.getString(R.string.keymap_key_mute);
		case KeyEvent.KEYCODE_ENDCALL:
			return context.getString(R.string.keymap_key_end_call);
		case KeyEvent.KEYCODE_CALL:
			return context.getString(R.string.keymap_key_call);
		case KeyEvent.KEYCODE_CLEAR:
			return context.getString(R.string.keymap_key_clear);
		case KeyEvent.KEYCODE_BUTTON_L1:
			return "L1";
		case KeyEvent.KEYCODE_BUTTON_L2:
			return "L2";
		case KeyEvent.KEYCODE_BUTTON_MODE:
			return context.getString(R.string.keymap_key_mode);
		case KeyEvent.KEYCODE_BUTTON_R1:
			return "R1";
		case KeyEvent.KEYCODE_BUTTON_R2:
			return "R2";
		case KeyEvent.KEYCODE_BUTTON_SELECT:
			return context.getString(R.string.keymap_key_select);
		case KeyEvent.KEYCODE_BUTTON_START:
			return context.getString(R.string.keymap_key_start);
		case KeyEvent.KEYCODE_BUTTON_THUMBL:
			return context.getString(R.string.keymap_key_thumb_left);
		case KeyEvent.KEYCODE_BUTTON_THUMBR:
			return context.getString(R.string.keymap_key_thumb_right);
		case KeyEvent.KEYCODE_ENTER:
			return context.getString(R.string.keymap_key_enter);
		case KeyEvent.KEYCODE_ESCAPE:
			return context.getString(R.string.keymap_key_escape);
		case 97: //Samsung Epic 4G
			return context.getString(R.string.keymap_key_emoticon);
		default:
			return (alt ? "Alt+" : "") + context.getString(R.string.keymap_unknown_key, code);
		}
	}

	public void onClick(DialogInterface dialog, int which) {
		if (which == -1) { //Clear
			String oldValue = getSharedPreferences().getString(getKey(),"");
			Preferences.getKeyMapper().clearKeyMap(oldValue);

			SharedPreferences.Editor ed = getSharedPreferences().edit();
			ed.putString(getKey(), "");
			ed.commit();
		}
		else {
			//Toast.makeText(context, "Cancel was clicked", Toast.LENGTH_SHORT).show();
		}
	}

	@Override
	protected Parcelable onSaveInstanceState() {

		// be free little dialog
		onActivityDestroy();

		return super.onSaveInstanceState();
	}
}
