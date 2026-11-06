package org.rephial.xyangband;

import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.NumberPicker;
import android.widget.PopupWindow;
import android.widget.SeekBar;
import android.widget.TextView;

public class QuantityPopup extends PopupWindow
        implements View.OnClickListener,
        SeekBar.OnSeekBarChangeListener,
        NumberPicker.OnValueChangeListener {

    public GameActivity context;

    public SeekBar valueBar = null;

    public NumberPicker valuePicker = null;

    public boolean locked = false;

    /** A dismissed, unconfirmed popup must cancel the pending native prompt. */
    private boolean resolved = false;
    private int sessionToken = 0;

    public ViewGroup root = null;

    public QuantityPopup(GameActivity p_context, String message,
                         int maxValue, int initialValue, int token) {
        super(p_context);

        // Prevent taps or keystrokes passing through to the game while choosing.
        setOutsideTouchable(false);
        setFocusable(true);

        context = p_context;

        setWidth(ViewGroup.LayoutParams.WRAP_CONTENT);
        setHeight(ViewGroup.LayoutParams.WRAP_CONTENT);

        root = (ViewGroup) context.getLayoutInflater().inflate
                (R.layout.quantity_layout, null);

        setContentView(root);

        valueBar = root.findViewById(R.id.valueBar);

        valuePicker = root.findViewById(R.id.valuePicker);

        valuePicker.setMinValue(0);

        configure(message, maxValue, initialValue, token);

        valueBar.setOnSeekBarChangeListener(this);

        valuePicker.setOnValueChangedListener(this);

        Button button = root.findViewById(R.id.allButton);
        button.setOnClickListener(this);

        button = root.findViewById(R.id.oneButton);
        button.setOnClickListener(this);

        button = root.findViewById(R.id.tenButton);
        button.setOnClickListener(this);

        button = root.findViewById(R.id.confirmButton);
        button.setOnClickListener(this);

        button = root.findViewById(R.id.cancelButton);
        button.setOnClickListener(this);

        setOnDismissListener(new OnDismissListener() {
            @Override
            public void onDismiss() {
                // Android Back and activity teardown must cancel pending prompt.
                if (!resolved) {
                    resolved = true;
                    StateManager state = context.getStateManager();
                    if (state != null) state.cancelQuantity(sessionToken);
                }
            }
        });
    }

    public int getSessionToken() {
        return sessionToken;
    }

    /** Native prompt ended; never echo Escape to a different game screen. */
    public void dismissSilently() {
        resolved = true;
        if (isShowing()) dismiss();
    }

    public void configure(String message, int maxValue, int initialValue, int token)
    {
        locked = true;
        sessionToken = token;
        resolved = false;
        int limit = Math.max(0, maxValue);

        valuePicker.setMaxValue(limit);
        valuePicker.setValue(QuantitySliderMath.clamp(initialValue, 0, limit));

        TextView prompt = root.findViewById(R.id.quantityPrompt);
        prompt.setText(message == null ? "" : message);

        Button button = root.findViewById(R.id.allButton);
        button.setText(context.getString(R.string.quantity_all_with_count, limit));
        button.setEnabled(limit > 0);

        button = root.findViewById(R.id.tenButton);
        button.setVisibility(limit <= 10 ? View.GONE : View.VISIBLE);

        button = root.findViewById(R.id.oneButton);
        button.setEnabled(limit > 0);

        button = root.findViewById(R.id.confirmButton);
        button.setEnabled(limit > 0);

        updateValueBar(valuePicker.getValue());
        locked = false;
    }

    @Override
    public void onClick(View v) {
        if (locked || resolved) return;
        locked = true;
        int id = v.getId();

        if (id == R.id.cancelButton) {
            resolved = true;
            StateManager state = context.getStateManager();
            if (state != null) state.cancelQuantity(sessionToken);
            dismiss();
            locked = false;
            return;
        }

        if (id == R.id.allButton) {
            valuePicker.setValue(valuePicker.getMaxValue());
        } else if (id == R.id.oneButton) {
            valuePicker.setValue(1);
        } else if (id == R.id.tenButton) {
            valuePicker.setValue(10);
        } else if (id != R.id.confirmButton) {
            locked = false;
            return;
        }

        // A failed or stale quantity cannot close the popup as "confirmed".
        // Only one deliberate button action can enqueue a complete response.
        if (sendValue()) {
            resolved = true;
            dismiss();
        }
        locked = false;
    }

    private boolean sendValue()
    {
        StateManager state = context.getStateManager();
        return state != null && state.enqueueQuantity(
                sessionToken, valuePicker.getValue(), valuePicker.getMaxValue());
    }

    @Override
    public void onProgressChanged(SeekBar seekBar,
                                  int progress, boolean fromUser)
    {
        if (locked || !fromUser) return;

        locked = true;
        valuePicker.setValue(QuantitySliderMath.valueForProgress(
                valuePicker.getMinValue(), valuePicker.getMaxValue(), progress));
        locked = false;
    }

    public void updateValueBar(int newVal)
    {
        valueBar.setProgress(QuantitySliderMath.progressForValue(
                valuePicker.getMinValue(), valuePicker.getMaxValue(), newVal));
    }

    @Override
    public void onStartTrackingTouch(SeekBar seekBar) {}

    @Override
    public void onStopTrackingTouch(SeekBar seekBar) {}

    @Override
    public void onValueChange(NumberPicker picker, int oldVal, int newVal)
    {
        if (locked) return;

        locked = true;

        updateValueBar(newVal);

        locked = false;
    }
}
