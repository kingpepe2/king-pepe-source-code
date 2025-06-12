package org.kingpepecore.qt;

import android.os.Bundle;
import android.system.ErrnoException;
import android.system.Os;

import org.qtproject.qt5.android.bindings.QtActivity;

import java.io.File;

public class KingpepeQtActivity extends QtActivity
{
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        final File kingpepeDir = new File(getFilesDir().getAbsolutePath() + "/.kingpepe");
        if (!kingpepeDir.exists()) {
            kingpepeDir.mkdir();
        }

        super.onCreate(savedInstanceState);
    }
}
