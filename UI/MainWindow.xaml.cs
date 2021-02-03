using Microsoft.WindowsAPICodePack.Dialogs;
using System;
using System.IO;
using System.Windows;
using UI.Utils;

namespace UI
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private RegistryWrapper registryWrapper = new RegistryWrapper();

        public MainWindow()
        {
            InitializeComponent();
            SaveLoadTxt.Text = registryWrapper.getSavePath("");
            TRFolderTxt.Text = registryWrapper.getTRPath("");
        }

        private void saveIni()
        {
            HandleIni.save(
                SaveLoadTxt.Text,
                GhostModeToogle.IsChecked.Value ? "write" : "read",
                TRFolderTxt.Text);
        }


        private void GhostModeToogle_Click(object sender, RoutedEventArgs e)
        {
            if(GhostModeToogle.IsChecked.Value)
            {
                GhostModeText.Text = "Save";
            } else
            {
                GhostModeText.Text = "Reproduce";
            }
            saveIni();
        }

        private void RunButton_Click(object sender, RoutedEventArgs e)
        {
            // Inject the DLL
            bool injected = DLL.inject();
            if (!injected)
            {
                MessageBox.Show("Couldn't inject the DLL. Is tomb2.exe running?");
                return;
            }
            MaterialDesignThemes.Wpf.ButtonProgressAssist.SetIsIndeterminate(RunButton, true);
            
        }

        private void SaveLoadFolderButton_Click(object sender, RoutedEventArgs e)
        {
            // Load SaveLoad folder
            CommonOpenFileDialog dialog = new CommonOpenFileDialog();
            dialog.InitialDirectory = registryWrapper.getSavePath("C:\\");
            dialog.IsFolderPicker = true;
            if (dialog.ShowDialog() == CommonFileDialogResult.Ok)
            {
                SaveLoadTxt.Text = dialog.FileName;
                registryWrapper.setSavePath(dialog.FileName);
            }
            saveIni();
        }

        private void TRFolderButton_Click(object sender, RoutedEventArgs e)
        {
            CommonOpenFileDialog dialog = new CommonOpenFileDialog();
            dialog.InitialDirectory = registryWrapper.getTRPath("C:\\");
            dialog.IsFolderPicker = true;
            if (dialog.ShowDialog() == CommonFileDialogResult.Ok)
            {
                //Check Tomb2.exe exists.
                string[] fileEntries = Directory.GetFiles(dialog.FileName);
                foreach(string file in fileEntries)
                {
                    if (file.EndsWith("Tomb2.exe"))
                    {
                        TRFolderTxt.Text = dialog.FileName;
                        registryWrapper.setTRPath(dialog.FileName);
                        return;
                    }
                }
                MessageBox.Show("The folder does not contains Tomb2.exe");
            }
            saveIni();
        }
    }
}
