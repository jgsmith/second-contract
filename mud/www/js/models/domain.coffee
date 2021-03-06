class SC.Models.Ward extends Backbone.Model
  idAttribute: "name"

  defaults: -> {}

  initialize: (options) ->
    console.log "Getting objects for ward"
    @objects = new SC.Collections.Thing [],
      domain: @collection.domain
      area: @collection.area
      ward: @id
    @objects.fetch()

  url: ->
    base = @collection.url() if @collection?
    base ?= "/"
    base + "/" + encodeURIComponent(@id)

class SC.Collections.Ward extends Backbone.Collection
  model: SC.Models.Ward
  initialize: (models, options) ->
    @domain = options.domain
    @area   = options.area
    console.log "Ward for ", @domain, @area
    @url = '/api/worldlib/ward/' + @domain + '/' + @area

class SC.Models.Area extends Backbone.Model
  defaults: -> {}

  initialize: (options) ->
    @domain = options.domain
    @scenes = new SC.Collections.Thing [],
      domain: @domain
      area: @id
      ward: 'scene'
    @paths = new SC.Collections.Thing [],
      domain: @domain
      area: @id
      ward: 'path'
    @terrains = new SC.Collections.Thing [],
      domain: @domain
      area: @id
      ward: 'terrain'
    @wards = new SC.Collections.Ward [],
      domain: @domain
      area: @id
    @scenes.fetch()
    @paths.fetch()
    @terrains.fetch()
    @wards.fetch()

  url: ->
    base = @collection.url() if @collection?
    base ?= "/"
    if @isNew()
      base
    else
      base + "/" + encodeURIComponent(@id)

class SC.Collections.Area extends Backbone.Collection
  initialize: (models, options) ->
    @domain = options.domain

  model: SC.Models.Area

  url: ->
    '/api/worldlib/area/' + @domain
    
class SC.Models.Domain extends Backbone.Model
  defaults: -> {}

  urlRoot: "/api/worldlib/domain"

  url: ->
    base = @urlRoot
    base ?= @collection.url if @collection?
    base ?= "/"
    if @isNew()
      base
    else
      base + "/" + encodeURIComponent(@id)

  initialize: ->
    if !@isNew()
      @areas = new SC.Collections.Area [],
        domain: @get('name')
      @areas.fetch()
      @areas.bind 'add', (area) =>
        @attributes.areas.push area.get('name')

  createArea: (name) ->
    if name? && name != "" && !(name in @get('areas'))
      @areas.create
        id: name
        name: name

class SC.Collections.Domain extends Backbone.Collection
  model: SC.Models.Domain
  url: '/api/worldlib/domain'

SC.Collections.domains = new SC.Collections.Domain
