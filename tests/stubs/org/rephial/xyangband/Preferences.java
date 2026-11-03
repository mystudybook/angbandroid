package org.rephial.xyangband;

/** Only the methods TermWindow needs to compile in JVM regression tests. */
public final class Preferences {
    private Preferences() { }
    public static int getTermWidth() { return 80; }
    public static int getTermHeight() { return 24; }
}
