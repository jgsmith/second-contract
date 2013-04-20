class SC.Models.Verb extends Backbone.Model
  defaults: -> {}

  urlRoot: "/api/iflib/verb"

  url: ->
    base = @urlRoot
    base ?= @collection.url if @collection?
    base ?= "/"
    if @isNew()
      base
    else
      base + "/" + encodeURIComponent(@id)

  initialize: ->

  validate: (attributes) ->
    if !attributes.verbs or !$.isArray(attributes.verbs) or attributes.verbs.length == 0
      return "A verb must have at least one word to identify it."

class SC.Collections.Verb extends Backbone.Collection
  model: SC.Models.Verb
  url: '/api/iflib/verb'

SC.Collections.verbs = new SC.Collections.Verb
