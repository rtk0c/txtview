# Considerations on doing real-time text rendering with proper shaping

Consider something like dear-imgui. To have proper text rendering, there are basically two options:
1. hijack something into its logic (or perhaps by designing an entirely new API) that caches the same text across frames, without doing hashtable lookups on the whole string
2. make shaping stupidly fast

Here we explore the viability of each option, and how it can be implemented.

# Making it fast

Chrome used to have a special codepath that detects Latin-1 only strings, and do a naive "shaping" pass over them rather than harfbuzz. They got rid of it because it wasn't significantly more performant than just harfbuzz.
If we want to do this, current `ImGui::DrawText()` still becomes doubles to 2-pass operation, first for scanning for Latin-1, second for actually drawing.
I do not think this is practical.

Alternatively, we can force the user to hint if a string is Latin-1 only.
Do note, this flag will probably be static for vast majority of the UI label texts, and always needs to be check-for-unicode for text fields. Again, not super useful.

# Cache it

Observe that most calls to `ImGui::DrawText()` will be static, for things like button labels, menu items, static labels.
The dyanmic calls comes from either `ImGui::Text()` with args for display data, or `ImGui::InputText()` (or the likes) for displaying user input.

TODO
