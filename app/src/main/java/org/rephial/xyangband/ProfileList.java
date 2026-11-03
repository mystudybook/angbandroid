package org.rephial.xyangband;

import java.util.ArrayList;

public class ProfileList extends ArrayList<Profile> {

	protected static String dl = "|";

	public ProfileList(){}

	public static ProfileList deserialize(String value) {
		String[] tk = value.split("\\"+dl);
		ProfileList pl = new ProfileList();
		for(int i=0; i < tk.length; i++)
			pl.add(Profile.deserialize(tk[i]));
		return pl;
	}

	public String serialize() {
		StringBuffer s = new StringBuffer();
		for(int i=0; i < this.size(); i++) {
		    if (s.length() > 0) s.append(dl);
		    s.append(this.get(i).serialize());
		}
		return s.toString();
	}

	public Profile findById(int id) {
		for(int ix = 0; ix < size(); ix++) {
			if (get(ix).id == id)
				return get(ix);
		}
		return null;
	}

	public Profile findByName(String name, int excludeId) {
		for(int ix = 0; ix < size(); ix++) {
			if (get(ix).id != excludeId 
				&& get(ix).name.compareTo(name) == 0)
				return get(ix);
		}
		return null;
	}

	public Profile findBySaveFile(String saveFile, int excludeId) {
		for(int ix = 0; ix < size(); ix++) {
			if (get(ix).id != excludeId 
				&& get(ix).saveFile.compareTo(saveFile) == 0)
				return get(ix);
		}
		return null;
	}

	// Resource IDs let the Activity apply its own locale to validation errors.
	public int validateChangeErrorResource(int id, String name, String saveFile) {
		if (name == null || name.length() == 0) return R.string.profile_validation_name_required;
		if (saveFile == null || saveFile.length() == 0) return R.string.profile_validation_savefile_required;
		if (findByName(name, id) != null) return R.string.profile_validation_duplicate_name;
		if (findBySaveFile(saveFile, id) != null) return R.string.profile_validation_duplicate_savefile;
		return 0;
	}

	// Preserve the existing English-returning method for compatibility.
	public String validateChange(int id, String name, String saveFile) {
		int code = validateChangeErrorResource(id, name, saveFile);
		if (code == R.string.profile_validation_name_required) return "Name is required";
		if (code == R.string.profile_validation_savefile_required) return "Save filename is required";
		if (code == R.string.profile_validation_duplicate_name) return "There is already a profile with that name";
		if (code == R.string.profile_validation_duplicate_savefile) return "There is already a profile using that save filename";
		return null;
	}

	public int getNextId() {
		int id = 0;
		for(int ix = 0; ix < size(); ix++) {
			if (get(ix).id > id) id = get(ix).id;
		}
		return id+1;
	}
}


