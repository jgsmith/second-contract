SC.Models.Domain = Backbone.Model.extend
  defaults: -> {}

  urlRoot: "http://localhost:6050/api/worldlib/domain"

  url: ->
    base = this.urlRoot
    base ?= this.collection.url if this.collection?
    base ?= "/"
    if this.isNew()
      return base;

    return base + "/" + encodeURIComponent(this.id)

  initialize: ->

SC.Collections.Domain = Backbone.Collection.extend
  model: SC.Models.Domain
  url: 'http://localhost:6050/api/worldlib/domain'

SC.Collections.domains = new SC.Collections.Domain
