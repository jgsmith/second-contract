Template Parser
===============

The template parser returns a LWO that can be used to render the template given
a data mapping and a point of view object. This parser can be used to render
the messages sent to various observers of actions.

The template consists of plain text with embedded directives. Grammar-based
directives are enclosed in angle brackets ('<' and '>') while non-grammar-based
directives are enclosed in double curly braces ('{{' and '}}').

Synopsis
--------

  # include <toollib.h>

  object parsed_template;
  string rendered_template

  parsed_template  = TEMPLATE_P -> parse_template("...");
  rendered_template = parsed_template->render(data_mapping, pov_object);

Grammar Directives
------------------

Grammar directives are one word or two words separated by a colon. If it is
a single word, then the word is either one of the participants in the action,
a case for the most recently mentioned participant, or a verb. If it is two
words, then the first word denotes the participant and the second word denotes
the case. The two word form can also be used to disambiguate verbs by specifying
a "verb" participant.

Participants:

* this - the object defining the template
* actor - the object initiating the action
* direct - the direct object(s) of the current action
* indirect - the indirect object(s) of the current action
* instrument - the instrument object(s) of the current action

Cases:

* name - the name of the participant
* nominative - the nominative pronoun for the participant
* objective - the objective pronoun for the participant
* possessive - the possessive pronoun for the participant
* possessive-noun - the possessive noun/name for the participant
* reflexive - the reflexive pronoun for the participant

For example, the following template would result in a number of messages:

  "<actor> <hit> <direct> with <instrument>."

From the actor's perspective:

  "You hit the nail with the hammer."

From the nail's perspective:

  "Bob hits you with the hammer."

From the hammer's perspective:

  "Bob hits the nail with you."
