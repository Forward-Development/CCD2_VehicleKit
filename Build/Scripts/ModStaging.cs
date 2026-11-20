using System;
using System.Collections.Generic;
using System.Linq;
using AutomationTool;
using EpicGames.Core;
using UnrealBuildTool;

/// <summary>
/// Stages non-uasset mod files (banks, configs, text, etc.) during DLC builds and outputs only the final .pak when archiving.
/// </summary>
public class ModStagingHandler : CustomStagingHandler
{
	private static readonly HashSet<string> ExcludedExtensions = new(StringComparer.OrdinalIgnoreCase)
	{
		".uasset",
		".umap",
		".uexp",
		".ubulk",
		".uptnl",
		".tmp",
		".bak"
	};

	/// <summary>
	/// Activates this handler only when packaging a valid DLC plugin.
	/// </summary>
	protected override bool TryInitialize(ProjectParams Params, DeploymentContext SC)
	{
		return Params.HasDLCName && Params.DLCFile != null && FileReference.Exists(Params.DLCFile);
	}

	/// <summary>
	/// Stages all non-cooked files from the DLC plugin's Content folder as UFS so UAT packs them into the DLC .pak.
	/// </summary>
	public override void GetFilesToStageForDLC(ProjectParams Params, DeploymentContext SC)
	{
		DirectoryReference ContentDir = DirectoryReference.Combine(Params.DLCFile.Directory, "Content");
		if (!DirectoryReference.Exists(ContentDir))
		{
			return;
		}

		List<FileReference> Files = SC.FindFilesToStage(ContentDir, StageFilesSearch.AllDirectories);
		foreach (FileReference File in Files)
		{
			string FileName = File.GetFileName();
			if (FileName.StartsWith(".", StringComparison.Ordinal))
			{
				continue;
			}

			string Ext = File.GetExtension();
			if (ExcludedExtensions.Contains(Ext))
			{
				continue;
			}

			SC.StageFile(StagedFileType.UFS, File);
		}
	}

	/// <summary>
	/// Replaces the full staged archive list with only the final DLC .pak at the archive root.
	/// </summary>
	public override void GetFilesToArchive(ProjectParams Params, DeploymentContext SC)
	{
		string PakSource = SC.ArchivedFiles.Keys.FirstOrDefault(
			PathStr => PathStr.EndsWith(".pak", StringComparison.OrdinalIgnoreCase));
		if (string.IsNullOrEmpty(PakSource))
		{
			return;
		}

		string PakName = $"{Params.DLCFile.GetFileNameWithoutExtension()}.pak";
		SC.ArchivedFiles.Clear();
		SC.ArchivedFiles.Add(PakSource, PakName);
	}
}
